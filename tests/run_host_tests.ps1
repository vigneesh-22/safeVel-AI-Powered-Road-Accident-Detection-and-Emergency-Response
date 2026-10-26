$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$compiler = Get-Command g++ -ErrorAction SilentlyContinue
if (-not $compiler) { throw 'g++ was not found. Install MinGW-w64 or run these tests in a C++ development environment.' }
& $compiler.Source -std=c++17 (Join-Path $PSScriptRoot 'test_detection.cpp') -o (Join-Path $env:TEMP 'safevel_detection_tests.exe')
& (Join-Path $env:TEMP 'safevel_detection_tests.exe')

