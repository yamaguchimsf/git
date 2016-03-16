name := "mllibTest"
version := "mllib-example"
scalaVersion := "2.10.4"
libraryDependencies ++=
	Seq("org.apache.spark" %% "spark-core" % "1.5.2" % "provided",
	    "org.apache.spark" %% "spark-mllib" % "1.5.2" % "provided",
	    "org.apache.spark" %% "spark-sql" % "1.5.2" % "provided",
	    "org.scalatest" %% "scalatest" % "1.9.1" % "test")
libraryDependencies ++=
	Seq("com.github.scopt" %% "scopt" % "3.3.0")
assemblyOption in assembly := (assemblyOption in assembly).value.copy(includeScala = false)
