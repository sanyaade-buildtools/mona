;; don't erase this line.
(let ((port (open-input-port "test/port.scm")))
  (assert-check-true "input-port? should be #t"
                     (input-port? (current-input-port))
                     (input-port? port))
  (close-input-port port))

(let ((port (open-output-port "test/tmp")))
  (assert-check-true "output-port? should be #t"
                     (output-port? (current-output-port))
                     (output-port? port))
  (close-output-port port))

(assert-check-false "output-port? should be #f"
                   (output-port? 1)
                   (output-port? #\a)
                   (output-port? #t)
                   (output-port? "test")
                   (output-port? (current-input-port))
                   (output-port? (quote x))
                   (output-port? (cons 1 2)))

(assert-check-false "input-port? should be #f"
                   (input-port? 1)
                   (input-port? #\a)
                   (input-port? #t)
                   (input-port? "test")
                   (input-port? (current-output-port))
                   (input-port? (quote x))
                   (input-port? (cons 1 2)))

(let* ((port (open-input-port "test/port.scm"))
       (c1 (read-char port))
       (c2 (read-char port))
       (c3 (read-char port))
       (c4 (read-char port)))
  (assert-check-true "read-char"
                     (char=? c1 #\;)
                     (char=? c2 #\;)
                     (char=? c3 #\space)
                     (char=? c4 #\d))
  (close-input-port port))

(let* ((port (open-input-port "test/port.scm"))
       (c1 (peek-char port))
       (c2 (peek-char port))
       (c3 (peek-char port))
       (c4 (peek-char port)))
  (assert-check-true "peek-char"
                     (char=? c1 #\;)
                     (char=? c2 #\;)
                     (char=? c3 #\;)
                     (char=? c4 #\;))
  (close-input-port port))

(let* ((port (open-output-port "test/tmp")))
  (write-char #\X port)
  (write-char #\Y port)
  (write-char #\Z port)
  (close-output-port port))

(let* ((port (open-input-port "test/tmp"))
       (c1 (read-char port))
       (c2 (read-char port))
       (c3 (read-char port))
       (c4 (read-char port)))
  (assert-check-true "read-char"
                     (char=? c1 #\X)
                     (char=? c2 #\Y)
                     (char=? c3 #\Z)
                     (eof-object? c4))
  (assert-check-false "eof-object?"
                      (eof-object? c1)
                      (eof-object? c2)
                      (eof-object? c3))
  (close-input-port port))

(let* ((port (open-output-port "test/tmp")))
  (display "test" port)
  (newline port)
  (close-output-port port))

(let* ((port (open-input-port "test/tmp"))
       (c1 (read-char port))
       (c2 (read-char port))
       (c3 (read-char port))
       (c4 (read-char port))
       (c5 (read-char port)))
  (assert-check-true "read-char"
                     (char=? c1 #\t)
                     (char=? c2 #\e)
                     (char=? c3 #\s)
                     (char=? c4 #\t)
                     (char=? c5 #\newline)))

(with-input-from-file "test/tmp" (lambda ()
                                   (let* ((c1 (read-char))
                                          (c2 (read-char))
                                          (c3 (read-char))
                                          (c4 (read-char))
                                          (c5 (read-char)))
                                   (assert-check-true "with-input-from-file"
                                                      (char=? c1 #\t)
                                                      (char=? c2 #\e)
                                                      (char=? c3 #\s)
                                                      (char=? c4 #\t)
                                                      (char=? c5 #\newline)))))

(with-output-from-file "test/tmp" (lambda ()
                                    (display "Hello")))


(call-with-input-file "test/tmp" (lambda (port)
                                   (let* ((c1 (read-char port))
                                          (c2 (read-char port))
                                          (c3 (read-char port))
                                          (c4 (read-char port))
                                          (c5 (read-char port)))
                                     (assert-check-true "call-with-input-file"
                                                        (char=? c1 #\H)
                                                        (char=? c2 #\e)
                                                        (char=? c3 #\l)
                                                        (char=? c4 #\l)
                                                        (char=? c5 #\o)))))

(call-with-output-file "test/tmp" (lambda (port)
                                    (display "Hi!" port)))

(call-with-input-file "test/tmp" (lambda (port)
                                   (let* ((c1 (read-char port))
                                          (c2 (read-char port))
                                          (c3 (read-char port)))
                                     (assert-check-true "call-with-input-file"
                                                        (char=? c1 #\H)
                                                        (char=? c2 #\i)
                                                        (char=? c3 #\!)))))

(call-with-input-file "test/tmp" (lambda (port)
                                   (assert-check-true "char-ready?"
                                                      (not (char-ready?))
                                                      (char-ready? port))))

;; read / write number
(call-with-output-file "test/read_write_tmp" (lambda (port)
                                               (write 10 port)))
(call-with-input-file "test/read_write_tmp" (lambda (port)
                                              (assert-check-true "read number"
                                               (= 10 (eval (read port) (scheme-report-environment 5)))
)))

;; read /write string
(call-with-output-file "test/read_write_tmp" (lambda (port)
                                               (write "test" port)))
(call-with-input-file "test/read_write_tmp" (lambda (port)
                                              (assert-check-true
                                               "read string"
                                               (string=? "test" (eval (read port) (scheme-report-environment 5)))
)))

;; read / write charcter and symbol
(define xyz "xyz-string")
(call-with-output-file "test/read_write_tmp" (lambda (port)
                                               (write #\c port)
                                               (write-char #\space port)
                                               (write (string->symbol "xyz") port)))
(call-with-input-file "test/read_write_tmp" (lambda (port)
                                              (assert-check-true
                                               "read charcter and symbol"
                                               (char=? #\c (eval (read port) (scheme-report-environment 5)))
                                               (string=? "xyz-string" (eval (read port) (scheme-report-environment 5)))
)))

;; read / write procedure call
(call-with-output-file "test/read_write_tmp" (lambda (port)
                                               (write '(+ 2 3) port)))
(call-with-input-file "test/read_write_tmp" (lambda (port)
                                              (assert-check-true
                                               "read procedure call"
                                               (= 5 (eval (read port) (scheme-report-environment 5))))))
