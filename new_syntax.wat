(module
    ;; WASI imports for I/O
    (import "wasi_snapshot_preview1" "fd_write"
      (func $fd_write (param i32 i32 i32 i32) (result i32)))
    (import "wasi_snapshot_preview1" "fd_read"
      (func $fd_read (param i32 i32 i32 i32) (result i32)))
    (import "wasi_snapshot_preview1" "proc_exit"
      (func $proc_exit (param i32)))

    ;; Memory (1 page = 64KB)
    (memory (export "memory") 1)

    ;; String data
    (data (i32.const 0) "=== Variable Declarations ===\00")
    (data (i32.const 30) "Enter a number: \00")
    (data (i32.const 47) "=== Format Output ===\00")
    (data (i32.const 69) "Converted to float\00")
    (data (i32.const 88) "Done!\00")

    ;; Helper: print string at offset with length
    (func $print_str (param $offset i32) (param $len i32)
        ;; Set up iovec at memory location 1024
        (i32.store (i32.const 1024) (local.get $offset))
        (i32.store (i32.const 1028) (local.get $len))
        ;; fd_write(stdout=1, iovec_ptr=1024, iovec_count=1, nwritten_ptr=1032)
        (drop (call $fd_write (i32.const 1) (i32.const 1024) (i32.const 1) (i32.const 1032)))
    )

    ;; Helper: print integer
    (func $print_int (param $n i32)
        (local $digit i32)
        (local $ptr i32)
        (local $neg i32)
        (local.set $ptr (i32.const 1100))
        ;; Handle negative
        (if (i32.lt_s (local.get $n) (i32.const 0))
          (then
              (local.set $neg (i32.const 1))
              (local.set $n (i32.sub (i32.const 0) (local.get $n)))
          )
        )
        ;; Convert digits (reverse order)
        (block $done
          (loop $loop
              (local.set $digit (i32.rem_u (local.get $n) (i32.const 10)))
              (local.set $n (i32.div_u (local.get $n) (i32.const 10)))
              (i32.store8 (local.get $ptr) (i32.add (local.get $digit) (i32.const 48)))
              (local.set $ptr (i32.add (local.get $ptr) (i32.const 1)))
              (br_if $loop (i32.gt_u (local.get $n) (i32.const 0)))
          )
        )
        ;; Add minus sign if negative
        (if (local.get $neg)
          (then
              (i32.store8 (local.get $ptr) (i32.const 45))
              (local.set $ptr (i32.add (local.get $ptr) (i32.const 1)))
          )
        )
        ;; Reverse and print
        (call $print_reversed (i32.const 1100) (local.get $ptr))
    )

    (func $print_reversed (param $start i32) (param $end i32)
        (local $len i32)
        (local $i i32)
        (local $j i32)
        (local $tmp i32)
        (local.set $len (i32.sub (local.get $end) (local.get $start)))
        (local.set $i (local.get $start))
        (local.set $j (i32.sub (local.get $end) (i32.const 1)))
        ;; Reverse in place
        (block $done
          (loop $loop
              (br_if $done (i32.ge_u (local.get $i) (local.get $j)))
              (local.set $tmp (i32.load8_u (local.get $i)))
              (i32.store8 (local.get $i) (i32.load8_u (local.get $j)))
              (i32.store8 (local.get $j) (local.get $tmp))
              (local.set $i (i32.add (local.get $i) (i32.const 1)))
              (local.set $j (i32.sub (local.get $j) (i32.const 1)))
              (br $loop)
          )
        )
        (call $print_str (local.get $start) (local.get $len))
    )

    ;; Entry point
    (func $_start (export "_start")
        (local $__tmp i32)
    (call $print_str (i32.const 0) (i32.const 29))
    (i32.store8 (i32.const 2000) (i32.const 10))
    (call $print_str (i32.const 2000) (i32.const 1))
    (call $print_int (local.get $x))
    (i32.store8 (i32.const 2000) (i32.const 10))
    (call $print_str (i32.const 2000) (i32.const 1))
    (call $print_int (local.get $name))
    (i32.store8 (i32.const 2000) (i32.const 10))
    (call $print_str (i32.const 2000) (i32.const 1))
    (call $print_int (local.get $x))
    (i32.store8 (i32.const 2000) (i32.const 10))
    (call $print_str (i32.const 2000) (i32.const 1))
    (call $print_str (i32.const 47) (i32.const 21))
    (i32.store8 (i32.const 2000) (i32.const 10))
    (call $print_str (i32.const 2000) (i32.const 1))
    (call $print_str (i32.const 69) (i32.const 18))
    (i32.store8 (i32.const 2000) (i32.const 10))
    (call $print_str (i32.const 2000) (i32.const 1))
    (call $print_str (i32.const 88) (i32.const 5))
    (i32.store8 (i32.const 2000) (i32.const 10))
    (call $print_str (i32.const 2000) (i32.const 1))
        (call $proc_exit (i32.const 0))
    )
)
