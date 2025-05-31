$scriptPath = $MyInvocation.MyCommand.Path
$scriptDir = Split-Path -Parent $scriptPath
$targetPath = Resolve-Path "$scriptDir/../atomixc/dist/bin/atomixc.js"
$zigDir = Resolve-Path "$scriptDir/../zig" -ErrorAction SilentlyContinue

if ($zigDir -ne $null -and (Test-Path $zigDir)) {
    $env:PATH += ";$zigDir"
}

function global:atomixc {
    node $targetPath @Args
}

function global:prompt {
    return "PS (atomixc) " + $(Get-Location) + "> "
}