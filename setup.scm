(display "loading setup...")
(load-extension "./nuklear")

(define (cleanup) 
 (begin (display "we are cleaning up now") (newline) (nuklear-stop)))

(define (callback) 
 (begin 
  (nuklear-begin "Window 1" 50 50 210 450)
      (nuklear-row-dynamic 25 1)
      (nuklear-slider freq "freq")
      (nuklear-slider gain "gain")
  (nuklear-end)))

(ps-eval 0 sine)

(ps-turnon 0)

(nuklear-start callback) 
(ps-set-shutdown-callback nuklear-stop)
