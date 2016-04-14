param( $SourceUrl, $FileNum, $OutPath, $FileName = "{0:000}.jpg", $StartNum = 1 )

$OutDir = new-item $OutPath -itemtype directory

$OutPath = $OutPath -replace "\[", "``["
$OutPath = $OutPath -replace "\]", "``]"
$OutPath = $OutPath.TrimEnd()

Push-Location -Path "$OutPath"
for( $Idx = $StartNum; $Idx -le $StartNum + $FileNum - 1; ++ $Idx )
{
	$UseName = [string]::Format( $FileName,$Idx)
	Write-Host ($SourceUrl + $UseName)
	Invoke-WebRequest ($SourceUrl + $UseName) -OutFile $UseName
}
Pop-Location
