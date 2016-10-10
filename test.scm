(load-extension "./nuklear")

(define (cleanup) (begin (display "we are cleaning up now") (newline)))

(define freq (ps-mkvar "freq" 0.5))
(define gain (ps-mkvar "gain" 0.5))

(define sine 
 "_freq get 0.01 port 200 1000 scale 
 _gain get 0.01 port 0 0.8 scale sine")

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
