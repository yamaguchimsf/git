$xmlobj = [xml](Get-Content .\hdfs-site.xml)
foreach ($property in $xmlobj.Configuration.property) {
    $str = $property.name + "`t" + $property.value | Out-File -Append out.tsv
}