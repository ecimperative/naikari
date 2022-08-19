set height 0
handle SIGTERM nostop print pass
handle SIGPIPE nostop
# Tell gdb to detach before a normal exit, so that LeakSanitizer can do its job.
break debug_enableLeakSanitizer
commands
   silent
   detach
   quit
end
echo .gdbinit: running naev with gdb wrapper\n
run

