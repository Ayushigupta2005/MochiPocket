# MochiPocket Compilation Script
Write-Host "🍡 Compiling MochiPocket..." -ForegroundColor Cyan

$CompilerPath = "C:\Program Files\CodeBlocks\MinGW\bin\g++.exe"
$SourceFiles = Get-Item src/*.cpp | ForEach-Object { $_.FullName }

$Args = @(
    "-std=c++17",
    "-static-libgcc",
    "-static-libstdc++",
    "-static",
    "-Iinclude",
    "-Ithirdparty/raylib-5.0_win64_mingw-w64/include",
    "-Lthirdparty/raylib-5.0_win64_mingw-w64/lib"
)

# Add source files to arguments
$Args += $SourceFiles

# Add libraries and output parameters
$Args += @(
    "-lraylib",
    "-lopengl32",
    "-lgdi32",
    "-lwinmm",
    "-o",
    "MochiPocket.exe"
)

if (!(Test-Path $CompilerPath)) {
    Write-Host "[ERROR] MinGW GCC not found at $CompilerPath" -ForegroundColor Red
    Write-Host "Please make sure Code::Blocks is installed under C:\Program Files\CodeBlocks." -ForegroundColor Yellow
    Exit 1
}

# Run compilation
Write-Host "Invoking: g++ $($Args -join ' ')" -ForegroundColor DarkGray
& $CompilerPath $Args

if ($LASTEXITCODE -eq 0) {
    Write-Host "✨ MochiPocket compiled successfully!" -ForegroundColor Green
    Write-Host "To run, execute: .\MochiPocket.exe" -ForegroundColor Yellow
} else {
    Write-Host "❌ Compilation failed with exit code $LASTEXITCODE" -ForegroundColor Red
}
