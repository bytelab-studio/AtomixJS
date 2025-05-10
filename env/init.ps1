$scriptPath = $MyInvocation.MyCommand.Path
$scriptDir = Split-Path -Parent $scriptPath
$targetPath = Resolve-Path "$scriptDir/../atomixc/dist/bin/atomixc.js"

function global:atomixc {
    node $targetPath @Args
}

function global:prompt {
    return "PS (atomixc) " + $(Get-Location) + "> "
}