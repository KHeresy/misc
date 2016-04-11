param( $OutPath, $SourceUrl, $FileNum, $FileName = "{0:000}.jpg", $StartNum = 1 )

new-item $OutPath -itemtype directory
for( $Idx = $StartNum; $Idx -le $StartNum + $FileNum - 1; ++ $Idx )
{
	$UseName = [string]::Format( $FileName,$Idx)
	Write-Host $UseName
	Invoke-WebRequest ($SourceUrl + $UseName) -OutFile ($OutPath + $UseName)
}
