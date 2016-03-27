package com.example

import org.apache.spark.{SparkConf, SparkContext}
import org.apache.spark.SparkContext._
import scopt.OptionParser
import java.text.BreakIterator
import scala.collection.mutable

import org.apache.spark.mllib.linalg.{Vector, Vectors}
import org.apache.spark.mllib.fpm.FPGrowth
import org.apache.spark.rdd.RDD

object MllibTest {

  case class Params(
    input: String = "/user/masahiro/data/mllib/sample_fpgrowth.txt",
    maxIterations: Int = 10,
    numPartitions: Int = 10,
    minConfidence: Double = 0.8,
    minSupport: Double = 0.2
  )

  def main (args: Array[String]){
    val defaultParams = Params()
    val parser = new OptionParser[Params]("sample"){
      head("test")
      opt[Int]("maxIterations")
        .text("maxIterations")
        .action((x, c) => c.copy(maxIterations = x))
      opt[Int]("numPartitions")
        .text("numPartitions")
        .action((x, c) => c.copy(numPartitions = x))
      opt[Double]("minSupport")
        .text("minSupport")
        .action((x, c) => c.copy(minSupport = x))
      opt[Double]("minConfidence")
        .text("minConfidence")
        .action((x, c) => c.copy(minConfidence = x))
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

    val data = sc.textFile(params.input) 

    val transactions : RDD[Array[String]] = data.map(s => s.trim.split(' '))

    val fpg = new FPGrowth()
      .setNumPartitions(params.numPartitions)
      .setMinSupport(params.minSupport)
    
    val model = fpg.run(transactions)

    model.freqItemsets.collect().foreach{ itemset =>
      println(itemset.items.mkString("[",",","]") + ", " + itemset.freq)
    }

    model.generateAssociationRules(params.minConfidence).collect().foreach { rule =>
      println(
        rule.antecedent.mkString("[", ",", "]")
          + " => " + rule.consequent.mkString("[", ",", "]") 
          + "," + rule.confidence)
    }
    sc.stop()
  }
}

