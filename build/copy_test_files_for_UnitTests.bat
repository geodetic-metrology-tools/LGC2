@echo off

FOR %%A IN (Release Debug) DO (
	REM copy test files
	xcopy ..\source\LGC_Tests\testFiles\Calc 		   		.\LGC_Tests\test_files\Calc  			>NUL /S /I /R /Y
	xcopy ..\source\LGC_Tests\testFiles\SimulationTests   .\LGC_Tests\test_files\SimulationTests  >NUL /S /I /R /Y
	xcopy ..\source\LGC_Tests\testFiles\MATHISCURRENT     .\LGC_Tests\test_files\MATHISCURRENT 	>NUL /S /I /R /Y
)
