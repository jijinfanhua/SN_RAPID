@echo off
setlocal enabledelayedexpansion

for /L %%i in (10000, 10000, 100000) do (
    for /L %%j in (100, 100, 2000) do (
        for /L %%k in (100000, 1, 100000) do (
            for /L %%x in (101, 1, 120) do (
                set /A "real_x=%%x/100"
                set "trace_name=syn_%%i_%%j_%%k_!real_x!.txt"
                echo RAPID-SIM.exe 1 !trace_name!
                echo get_average_queue_length.py !trace_name!
            )
        )
    )
)

endlocal
