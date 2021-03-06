
;;;; -*- Emacs-Lisp -*-
;;;; Basic Configuration for Emacs 21/22
;;;;

;; ;;; Language Environment
;; (set-language-environment "Japanese")
;; (setq default-input-method "Janpanese-anthy")

;; ;(set-default-coding-systems 'euc-jp)
;; (set-default-coding-systems 'utf-8)

;; (set-buffer-file-coding-system 'euc-jp-unix)
;; (set-terminal-coding-system 'euc-jp)

;; (set-keyboard-coding-system 'euc-jp)

;; ;;; Input Method for Japanese
;; (cond
;; ;; IIIMECF + ATOKX2
;; ((locate-library "iiimcf-sc")
;; (require 'iiimcf-sc)
;; (setq iiimcf-server-control-default-language "ja")
;; (setq default-input-method 'iiim-server-control)

;; ;; color display
;; (setq iiimcf-UI-preedit-use-face-p t)

;; ;; Canna-like key bindings (not perfect)
;; (defun define-iiimcf-preedit-key-trans (src dst)
;; (define-key iiimcf-server-control-preedit-state-keymap src
;; `(lambda () (interactive) (iiimcf-server-control-keyforward ',dst))
;; ))
;; (define-iiimcf-preedit-key-trans "\C-a" 'home)
;; (define-iiimcf-preedit-key-trans "\C-b" 'left )
;; (define-iiimcf-preedit-key-trans "\C-d" 'delete )
;; (define-iiimcf-preedit-key-trans "\C-e" 'end )
;; (define-iiimcf-preedit-key-trans "\C-f" 'right )
;; (define-iiimcf-preedit-key-trans "\C-g" 'escape )
;; (define-iiimcf-preedit-key-trans "\C-h" 'backspace )
;; (define-iiimcf-preedit-key-trans "\C-i" 'S-left )
;; (define-iiimcf-preedit-key-trans "\C-m" 'return )
;; (define-iiimcf-preedit-key-trans "\C-n" 'down )
;; (define-iiimcf-preedit-key-trans "\C-o" 'S-right )
;; (define-iiimcf-preedit-key-trans "\C-p" 'up )
;; )

;; ;; ;; Tamago + Canna
;; ;; ((and (locate-library "egg")
;; ;; (or (locate-library "egg-canna") (locate-library "canna")))
;; ;; (set-input-method "japanese-egg-canna")
;; ;; (setq canna-hostname "localhost")
;; ;; ;; Canna-like key bindings (uncomment if wanted)
;; ;; ;(define-key its-mode-map "\C-h" 'its-delete-backward-SYL )
;; ;; ;(define-key its-mode-map "\C-g" 'its-cancel-input )
;; ;; ;(define-key egg-conversion-map "\C-g" 'egg-abort-conversion )
;; ;; )

;; ;; KKC (fallback)
;; ((set-input-method "japanese"))
;; )

;; ;; mail
;; (setq mew-pop-user "yamaguchi.m.ab@m.titech.ac.jp")

;; ;; author
;; (setq mew-name user-full-name)
;; (setq mew-user user-login-name)
;; (setq mew-mail-domain "mi.cs.titech.ac.jp")

;; ;; don't delete mail from server
;; (setq mew-pop-delete nil)

;; ;; default
;; (setq mew-pop-auth 'pass)
;; (setq mew-pop-ssl t)
;; (setq mew-pop-ssl-port "995")
;; (setq mew-pop-server "mail.nap.gsic.titech.ac.jp")

;; (setq mew-smtp-user mew-pop-user)
;; (setq mew-smtp-ssl t)
;; (setq mew-smtp-ssl-port "465")
;; (setq mew-smtp-server "mail.nap.gsic.titech.ac.jp")

;; (setq mew-ssl-verify-level 0)
;; (setq mew-ssl-ver 3)
;; (custom-set-variables
;; ;; custom-set-variables was added by Custom.
;; ;; If you edit it by hand, you could mess it up, so be careful.
;; ;; Your init file should contain only one such instance.
;; ;; If there is more than one, they won't work right.
;; '(case-fold-search nil))
;; (custom-set-faces
;; ;; custom-set-faces was added by Custom.
;; ;; If you edit it by hand, you could mess it up, so be careful.
;; ;; Your init file should contain only one such instance.
;; ;; If there is more than one, they won't work right.
;; )


;;;;my settings

;;start message
(setq inhibit-startup-message t)

;;???
;(put 'set-goal-column 'disabled nil)

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
;; (set-face-foreground 'font-lock-string-face "purple")
(set-face-foreground 'font-lock-keyword-face "yellow1")
(set-face-foreground 'font-lock-function-name-face "aquamarine1")
(set-face-bold-p 'font-lock-function-name-face t)
;; (set-face-foreground 'font-lock-variable-name-face "black")
;; (set-face-foreground 'font-lock-type-face "LightSeaGreen")
;; (set-face-foreground 'font-lock-builtin-face "purple")
;; (set-face-foreground 'font-lock-constant-face "black")
;; (set-face-foreground 'font-lock-warning-face "red")
;; (set-face-bold-p 'font-lock-warning-face nil)
))

;;location of scroll bar
(set-scroll-bar-mode nil)

;;display on scroll bar
(display-time-mode 1)
(line-number-mode 1)
(column-number-mode 1)
(tool-bar-mode -1)


;;emphasize the TAB
;(defface my-face-b-1 '((t (:background "darkgreenx" :underline t))) nil)
(defface my-face-b-2 '((t (:background "cyan" :underline t))) nil)
;(defface my-face-b-3 '((t (:background "aquamarine"))) nil)
(defface my-face-u-1 '((t (:foreground "aquamarine" :underline t))) nil)
(defvar my-face-b-1 'my-face-b-1)
(defvar my-face-b-2 'my-face-b-2)
;(defvar my-face-b-3 'my-face-b-3)
(defvar my-face-u-1 'my-face-u-1)

(defadvice font-lock-mode (before my-font-lock-mode ())
(font-lock-add-keywords
major-mode
'(
; (" " 0 my-face-b-1 append)
("\t" 0 my-face-b-2 append)
; ("\t" 0 my-face-b-3 append)
("[ \t]+$" 0 my-face-u-1 append)
)))
(ad-enable-advice 'font-lock-mode 'before 'my-font-lock-mode)
(ad-activate 'font-lock-mode)

;alter the TAB to 4-spaces
;(setq-default tab-width 4 ident-tabs-mode nil)


;print row number
;(require 'linum)
;(global-linum-mode)

(setq-default tab-width 2 indent-tabs-mode nil)

;;; reverse action of C-x o
(define-key ctl-x-map "p"
#'(lambda (arg) (interactive "p") (other-window (- arg))))

(put 'upcase-region 'disabled nil)

