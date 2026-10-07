# bug_report.ps1 - see bug_report.bat. Nothing is uploaded; the zip stays on your disk.
param([switch]$WithSave)
if ($args -contains '--with-save') { $WithSave = $true }
$base = if ($env:MH1_SAVE_DIR) { Split-Path -Parent $env:MH1_SAVE_DIR } else { Join-Path $env:APPDATA 'mh1pc' }
$logs = if ($env:MH1_LOG_DIR) { $env:MH1_LOG_DIR } else { Join-Path $base 'logs' }
$card = if ($env:MH1_SAVE_DIR) { $env:MH1_SAVE_DIR } else { Join-Path $base 'memcard0' }
$stamp = Get-Date -Format 'yyyyMMdd_HHmmss'
$tmp = Join-Path $env:TEMP "mh1_bug_$stamp"
New-Item -ItemType Directory -Force -Path $tmp | Out-Null
$newest = Get-ChildItem -Path $logs -Filter 'mh1_*.log' -ErrorAction SilentlyContinue | Sort-Object Name -Descending | Select-Object -First 2
if (-not $newest) { Write-Host "No debug log found in $logs - run the game once first."; exit 1 }
foreach ($f in $newest) { Copy-Item $f.FullName $tmp }
$meta = Join-Path $tmp 'save_info.txt'
"Save folder listing (names, sizes, modified times; no contents)" | Out-File $meta -Encoding utf8
if (Test-Path $card) {
    Get-ChildItem -Path $card -Recurse | ForEach-Object {
        $rel = $_.FullName.Substring($card.Length).TrimStart('\')
        '{0}  {1} bytes  {2}' -f $rel, $(if ($_.PSIsContainer) { 'dir' } else { $_.Length }), $_.LastWriteTime.ToString('s')
    } | Out-File $meta -Append -Encoding utf8
} else { "(no save folder)" | Out-File $meta -Append -Encoding utf8 }
if ($WithSave -and (Test-Path $card)) { Copy-Item $card (Join-Path $tmp 'save') -Recurse }
$zip = Join-Path (Get-Location) "mh1_bug_report_$stamp.zip"
Compress-Archive -Path (Join-Path $tmp '*') -DestinationPath $zip -Force
Remove-Item $tmp -Recurse -Force
Write-Host ""
Write-Host "Made $zip"
Write-Host "It holds: $($newest.Name -join ', '), save_info.txt$(if ($WithSave) {', and your save'} else {' (your save is NOT included)'})"
Write-Host "The logs hold no user name or home path; check them yourself if you like (they are plain text)."
Write-Host ""
Write-Host "Open a bug report and attach the zip:"
Write-Host "  https://github.com/itsbigcforme67/mh1-xbox/issues/new?template=bug_report.md"
