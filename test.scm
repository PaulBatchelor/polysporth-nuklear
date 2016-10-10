
(define freq (ps-mkvar "freq" 0.6))
(define gain (ps-mkvar "gain" 0.4))

(define sine 
 "_freq get 0.01 port 200 1000 scale 
 _gain get 0.01 port 0 0.8 scale sine")

(define (init_time) 
 (begin 
  (display "init time") (newline)
  (load "setup.scm")))

(ps-set-init-callback init_time) 
