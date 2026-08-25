$ErrorActionPreference = "Stop"

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
$builder = Join-Path $repoRoot "python_scripts\build_weapon_art_atlas.py"
& python $builder --check
if ($LASTEXITCODE -ne 0) {
    throw "Weapon Art atlas validation failed."
}

$distRoot = Join-Path $PSScriptRoot "dist"
$imagesOut = Join-Path $distRoot "SKSE\Plugins\SpellHotbar\images"
$artdataOut = Join-Path $distRoot "SKSE\Plugins\SpellHotbar\artdata"
New-Item -ItemType Directory -Force -Path $imagesOut, $artdataOut | Out-Null

$imagesSource = Join-Path $repoRoot "data\SKSE\Plugins\SpellHotbar\images"
$artdataSource = Join-Path $repoRoot "data\SKSE\Plugins\SpellHotbar\artdata\arts_ashes.csv"
Copy-Item -LiteralPath (Join-Path $imagesSource "icons_weapon_arts.png") -Destination $imagesOut -Force
Copy-Item -LiteralPath (Join-Path $imagesSource "icons_weapon_arts.csv") -Destination $imagesOut -Force
Copy-Item -LiteralPath $artdataSource -Destination $artdataOut -Force

Write-Host "Built isolated Weapon Art icon runtime package at $distRoot"
