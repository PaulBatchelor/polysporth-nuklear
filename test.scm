(load-extension "./nuklear")

(define (cleanup) (begin (display "we are cleaning up now") (newline)))

(nuklear-start)
(ps-set-shutdown-callback nuklear-stop)
