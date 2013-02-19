;;;; -*- Emacs-Lisp -*-
;;;; Basic Configuration for Emacs 21/22
;;;;


;;;;original

;;start message
(setq inhibit-startup-message t)

;;show corresponding paren
(show-paren-mode 1)

;;frame settings
(setq initial-frame-alist
 (append
 '((width . 235)
 (height . 64)
 (left . 1)
 (top . 1)
 )initial-frame-alist))
;(setq default-frame-alist initial-frame-alist)
;(set-frame-parameter nil 'fullscreen 'fullboth)

;;; fullscreen
;(set-frame-parameter nil 'fullscreen 'fullboth)

;;;; not change background color
(transient-mark-mode 0)

;; not make ~ files
(setq backup-inhibited t)

;;color settings
(if window-system (progn
(add-to-list 'default-frame-alist '(foreground-color . "White"))
(add-to-list 'default-frame-alist '(background-color . "DarkGreen"))
(add-to-list 'default-frame-alist '(cursor-color . "SlateBlue3"))
(add-to-list 'default-frame-alist '(mouse-color . "SlateBlue2"))

(set-face-foreground 'modeline "white")
(set-face-background 'modeline "MediumPurple2")
(set-face-background 'region "Blue")
(set-face-foreground 'mode-line-inactive "black")
(set-face-background 'mode-line-inactive "SteelBlue1")

(set-face-foreground 'font-lock-comment-face "orange1")
(set-face-foreground 'font-lock-keyword-face "yellow1")
(set-face-foreground 'font-lock-function-name-face "aquamarine1")
(set-face-bold-p 'font-lock-function-name-face t)
))

;;location of scroll bar
(set-scroll-bar-mode nil)

;;display on scroll bar
(display-time-mode 1)
(line-number-mode 1)
(column-number-mode 1)
(tool-bar-mode -1)

;;emphasize the TAB
(defface my-face-b-2 '((t (:background "cyan" :underline t))) nil)
(defface my-face-u-1 '((t (:foreground "aquamarine" :underline t))) nil)
(defvar my-face-b-1 'my-face-b-1)
(defvar my-face-b-2 'my-face-b-2)
(defvar my-face-u-1 'my-face-u-1)

(defadvice font-lock-mode (before my-font-lock-mode ())
(font-lock-add-keywords
major-mode
'(
("\t" 0 my-face-b-2 append)
("[ \t]+$" 0 my-face-u-1 append)
)))
(ad-enable-advice 'font-lock-mode 'before 'my-font-lock-mode)
(ad-activate 'font-lock-mode)

(setq-default tab-width 2 indent-tabs-mode nil)

;;; reverse action of C-x o
(define-key ctl-x-map "p"
#'(lambda (arg) (interactive "p") (other-window (- arg))))

(put 'upcase-region 'disabled nil)

