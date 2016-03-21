#!/bin/bash

sudo -u hdfs hdfs dfs -mkdir /user
sudo -u hdfs hdfs dfs -chown hdfs:hadoop /user
sudo -u hdfs hdfs dfs -chmod 777 /user
sudo -u hdfs hdfs dfs -mkdir /user/hdfs
sudo -u hdfs hdfs dfs -chown hdfs:hdfs /user/hdfs
sudo -u hdfs hdfs dfs -chmod 755 /user/hdfs
sudo -u hdfs hdfs dfs -mkdir /user/mapred
sudo -u hdfs hdfs dfs -chown mapred /user/mapred
sudo -u hdfs hdfs dfs -chmod 755 /user/mapred
sudo -u hdfs hdfs dfs -mkdir /user/yarn
sudo -u hdfs hdfs dfs -chown yarn:yarn /user/yarn
sudo -u hdfs hdfs dfs -chmod 755 /user/yarn
sudo -u hdfs hdfs dfs -mkdir /user/history
sudo -u hdfs hdfs dfs -chown mapred:hadoop /user/history
sudo -u hdfs hdfs dfs -chmod 1777 /user/history
sudo -u hdfs hdfs dfs -mkdir /tmp
sudo -u hdfs hdfs dfs -chown hdfs:hdfs /tmp
sudo -u hdfs hdfs dfs -chmod 1777 /tmp
sudo -u hdfs hdfs dfs -mkdir /mapred
sudo -u hdfs hdfs dfs -chown mapred:mapred /mapred
sudo -u hdfs hdfs dfs -chmod 755 /mapred
sudo -u hdfs hdfs dfs -mkdir /mapred/system
sudo -u hdfs hdfs dfs -chown mapred:mapred /mapred/system
sudo -u hdfs hdfs dfs -chmod 755 /mapred/system
sudo -u hdfs hdfs dfs -mkdir /mapred/staging
sudo -u hdfs hdfs dfs -chown mapred:mapred /mapred/staging
sudo -u hdfs hdfs dfs -chmod 700 /mapred/staging
sudo -u hdfs hdfs dfs -mkdir /var
sudo -u hdfs hdfs dfs -chown hdfs:hadoop /var
sudo -u hdfs hdfs dfs -chmod 755 /var
sudo -u hdfs hdfs dfs -mkdir /var/log
sudo -u hdfs hdfs dfs -chown hdfs:hadoop /var/log
sudo -u hdfs hdfs dfs -chmod 755 /var/log
sudo -u hdfs hdfs dfs -mkdir /var/log/spark
sudo -u hdfs hdfs dfs -chown spark:spark /var/log/spark
sudo -u hdfs hdfs dfs -chmod 777 /var/log/spark
