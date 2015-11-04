@echo off

FOR %%A IN (Release Debug) DO (
	REM copy test files
	xcopy ..\source\tests\Calc 		   		.\tests\test_files\Calc  			>NUL /S /I /R /Y
	xcopy ..\source\tests\SimulationTests   .\tests\test_files\SimulationTests  >NUL /S /I /R /Y
	xcopy ..\source\tests\MATHISCURRENT     .\tests\test_files\MATHISCURRENT 	>NUL /S /I /R /Y
)
