# Vramsteg ('progress' in Svenska, almost)

Vramsteg is an open source, command line utility that provides shell scripts
with a full-featured progress indicator. The progress bar can display elapsed
time, remaining time estimate, percentage completed, and labels. The progress
bar can have user-specified colors, and be rendered in multiple styles.

See the man page (enter 'man vramsteg') for full details on how to use vramsteg
or look in the srv/examples directory for several shell scripts that illustrate
usage.

---

# -- MIRROR README --

The goal of this repository is to host vramsteg on Github, so that it can be
installed as Zshell plugin, with following commands added to ~/.zshrc:

```zsh
zi ice has'cmake;make' as'command' pick'src/vramsteg' \
  atclone'cmake . ; cp -vf doc/man/vramsteg.1 ${ZI[MAN_DIR]}/man1' atpull'%atclone' make
zi light z-shell/vramsteg-zsh
```
