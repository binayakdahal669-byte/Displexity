# Run this as Administrator to fix your PATH
# Right-click PowerShell -> Run as Administrator -> then run this script

$correctPath = @(
    "C:\Program Files\Common Files\Oracle\Java\javapath",
    "C:\WINDOWS\system32",
    "C:\WINDOWS",
    "C:\WINDOWS\System32\Wbem",
    "C:\WINDOWS\System32\WindowsPowerShell\v1.0",
    "C:\WINDOWS\System32\OpenSSH",
    "C:\Users\binay\AppData\Local\Programs\Python\Python314\Scripts",
    "C:\Program Files\dotnet",
    "C:\Program Files\Neovim\bin",
    "C:\Program Files\nodejs",
    "C:\Users\binay\AppData\Local\Programs\Python\Python314",
    "C:\msys64\ucrt64\bin",
    "C:\Program Files\Void\bin",
    "C:\msys64\ucrt64",
    "C:\Program Files (x86)\GnuWin32\bin",
    "C:\Program Files (x86)\Windows Kits\10\Windows Performance Toolkit",
    "C:\Python314\Scripts",
    "C:\Python314",
    "C:\Users\binay\.cargo\bin",
    "C:\Users\binay\AppData\Local\Microsoft\WindowsApps",
    "C:\Users\binay\AppData\Local\Programs\Warp\bin",
    "C:\Users\binay\AppData\Roaming\npm",
    "C:\Users\binay\AppData\Local\Programs\Microsoft VS Code\bin",
    "C:\Users\binay\AppData\Local\Programs\Kiro\bin",
    "C:\Program Files\Displexity\bin"
) -join ";"

Write-Host "Setting PATH to:" -ForegroundColor Yellow
Write-Host $correctPath

[Environment]::SetEnvironmentVariable("PATH", $correctPath, "Machine")

Write-Host "`nPATH has been restored!" -ForegroundColor Green
Write-Host "Please restart your terminal/computer for changes to take effect."

# Verify
Write-Host "`nVerifying..." -ForegroundColor Cyan
$newPath = [Environment]::GetEnvironmentVariable("PATH", "Machine")
Write-Host $newPath