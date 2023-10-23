#default values
source_dir ?= ~/workspace
BACKUP ?= ~/work_backup
SECS ?= 60
MAX ?= 10

source_dir ?= $(SOURCE)
backup_dir ?= $(BACKUP)
interval_secs ?= $(SECS)
max_backups ?= $(MAX)

.PHONY: backup

backup: $(backup_dir) backupd.sh
	./backupd.sh $(source_dir) $(backup_dir) $(interval_secs) $(max_backups)

# Create the backup directory if not existed
$(backup_dir):
	mkdir -p $(backup_dir)
