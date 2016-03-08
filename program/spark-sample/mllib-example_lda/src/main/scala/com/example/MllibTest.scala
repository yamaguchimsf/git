package com.example

import org.apache.spark.{SparkConf, SparkContext}
import org.apache.spark.SparkContext._
import scopt.OptionParser
import java.text.BreakIterator
import scala.collection.mutable

import org.apache.spark.mllib.linalg.{Vector, Vectors}
import org.apache.spark.mllib.clustering.{LDA, EMLDAOptimizer, DistributedLDAModel}
import org.apache.spark.sql.types.{StructField, StructType}
import org.apache.spark.sql.{Row, SQLContext}
import org.apache.spark.rdd.RDD

object MllibTest {

  case class Params(
    //input: String = "/user/cti/mllib_input/sample_lda_data.txt",
    input: Seq[String] = Seq.empty,
    //input: String = "/user/cti/mllib_input/docword.kos.txt.gz",
    lambda: Double = 1.0,
    algorithm: String = "em",
    vocabSize: Int = 10000,
    k: Int = 3,
    checkpointDir: Option[String] = None,
    maxIterations: Int = 10
  )

  def main (args: Array[String]){
    val defaultParams = Params()
    val parser = new OptionParser[Params]("sample"){
      head("test")
      opt[Double]("lambda")
        .text("lambda")
        .action((x, c) => c.copy(lambda = x))
      opt[Int]("vocabSize")
        .text("vocabSize")
        .action((x, c) => c.copy(vocabSize = x))
      opt[Int]("maxIterations")
        .text("maxIterations")
        .action((x, c) => c.copy(maxIterations = x))
      opt[Int]("k")
        .text("k")
        .action((x, c) => c.copy(k = x))
      opt[String]("algorithm")
        .text("algorithm")
        .action((x, c) => c.copy(algorithm = x))
      arg[String]("input")
        .text("input path")
        .action((x, c) => c.copy(input = c.input :+ x))
    }

    parser.parse(args, defaultParams).map { params =>
      run(params)
    }.getOrElse {
      sys.exit(1)
    }
  }

  def run(params: Params){
    val conf = new SparkConf()
    val sc = new SparkContext(conf)

    // Load documents, and prepare them for LDA.
    val preprocessStart = System.nanoTime()
    val (corpus, vocabArray, actualNumTokens) =
      preprocess(sc, params.input, params.vocabSize)
    corpus.cache()

    val actualCorpusSize = corpus.count()
    val actualVocabSize = vocabArray.size
    val preprocessElapsed = (System.nanoTime() - preprocessStart) / 1e9

    println()
    println(s"Corpus summary:")
    println(s"\t Training set size: $actualCorpusSize documents")
    println(s"\t Vocabulary size: $actualVocabSize terms")
    println(s"\t Training set size: $actualNumTokens tokens")
    println(s"\t Preprocessing time: $preprocessElapsed sec")
    println()


    // Run LDA.
    val lda = new LDA()

    val optimizer = params.algorithm.toLowerCase match {
      case "em" => new EMLDAOptimizer
//      // add (1.0 / actualCorpusSize) to MiniBatchFraction be more robust on tiny datasets.
//      case "online" => new OnlineLDAOptimizer().setMiniBatchFraction(0.05 + 1.0 / actualCorpusSize)
//      case _ => throw new IllegalArgumentException(
//        s"Only em, online are supported but got ${params.algorithm}.")
    }

    lda.setOptimizer(optimizer)
      .setK(params.k)
      .setMaxIterations(params.maxIterations)
      //.setDocConcentration(params.docConcentration)
      //.setTopicConcentration(params.topicConcentration)
      //.setCheckpointInterval(params.checkpointInterval)
    if (params.checkpointDir.nonEmpty) {
      sc.setCheckpointDir(params.checkpointDir.get)
    }
    val startTime = System.nanoTime()
    val ldaModel = lda.run(corpus)
    val elapsed = (System.nanoTime() - startTime) / 1e9

    println(s"Finished training LDA model.  Summary:")
    println(s"\t Training time: $elapsed sec")

    if (ldaModel.isInstanceOf[DistributedLDAModel]) {
      val distLDAModel = ldaModel.asInstanceOf[DistributedLDAModel]
      val avgLogLikelihood = distLDAModel.logLikelihood / actualCorpusSize.toDouble
      println(s"\t Training data average log likelihood: $avgLogLikelihood")
      println()
    }

    // Print the topics, showing the top-weighted terms for each topic.
    val topicIndices = ldaModel.describeTopics(maxTermsPerTopic = 10)
    val topics = topicIndices.map { case (terms, termWeights) =>
      terms.zip(termWeights).map { case (term, weight) => (vocabArray(term.toInt), weight) }
    }
    println(s"${params.k} topics:")
    topics.zipWithIndex.foreach { case (topic, i) =>
      println(s"TOPIC $i")
      topic.foreach { case (term, weight) =>
        println(s"$term\t$weight")
      }
      println()
    }

    sc.stop()
  }

  def preprocess(
    sc: SparkContext,
    paths: Seq[String],
    vocabSize: Int): (RDD[(Long, Vector)], Array[String], Long) = {
    
    val textRDD: RDD[String] = sc.textFile(paths.mkString(","))

    val tokenizer = new SimpleTokenizer(sc)
    //val tokenized: RDD[(Long, IndexedSeq[String])] = textRDD.zipWithIndex().map { case (text, id) => id -> tokenizer.getWords(text)}
    val tokenized: RDD[(Long, IndexedSeq[String])] = textRDD.zipWithIndex().map { case (text, id) => id -> tokenizer.getWords(text)}
    tokenized.cache()

    // Counts words: RDD[(word, wordCount)]
    val wordCounts: RDD[(String, Long)] = tokenized
      .flatMap { case (_, tokens) => tokens.map(_ -> 1L) }
      .reduceByKey(_ + _)
    wordCounts.cache()
    val fullVocabSize = wordCounts.count()
    // Select vocab
    //  (vocab: Map[word -> id], total tokens after selecting vocab)
    val (vocab: Map[String, Int], selectedTokenCount: Long) = {
      val tmpSortedWC: Array[(String, Long)] = if (vocabSize == -1 || fullVocabSize <= vocabSize) {
        // Use all terms
        wordCounts.collect().sortBy(-_._2)
      } else {
        // Sort terms to select vocab
        wordCounts.sortBy(_._2, ascending = false).take(vocabSize)
      }
      (tmpSortedWC.map(_._1).zipWithIndex.toMap, tmpSortedWC.map(_._2).sum)
    }

    val documents = tokenized.map { case (id, tokens) =>
      // Filter tokens by vocabulary, and create word count vector representation of document.
      val wc = new mutable.HashMap[Int, Int]()
      tokens.foreach { term =>
        if (vocab.contains(term)) {
          val termIndex = vocab(term)
          wc(termIndex) = wc.getOrElse(termIndex, 0) + 1
        }
      }
      val indices = wc.keys.toArray.sorted
      val values = indices.map(i => wc(i).toDouble)

      val sb = Vectors.sparse(vocab.size, indices, values)
      (id, sb)
    }

    val vocabArray = new Array[String](vocab.size)
    vocab.foreach { case (term, i) => vocabArray(i) = term }

    (documents, vocabArray, selectedTokenCount)
  }
}

private class SimpleTokenizer(sc: SparkContext) extends Serializable {
  private val allWordRegex = "^(\\p{L}*)$".r
  private val minWordLength = 3

  def getWords(text: String): IndexedSeq[String] = {
    val words = new mutable.ArrayBuffer[String]()
    val wb = BreakIterator.getWordInstance
    wb.setText(text)

    var current = wb.first()
    var end = wb.next()
    while(end != BreakIterator.DONE) {
      val word: String = text.substring(current, end).toLowerCase
      word match {
        case allWordRegex(w) if w.length >= minWordLength =>
          words += w
        case _ =>
      }

      current = end
      try {
        end = wb.next()
      } catch {
        case e: Exception =>
          end = BreakIterator.DONE
      }
    }
    words
  }
}
