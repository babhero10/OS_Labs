: '
    File:        backup-cron.sh
    Author:      Abdullah Elsayed Ahmed
    ID:          7459
    Description: Automatically backup folder using cron
'

#*********** Validate arguments ***********#
# Validate number of arguments
if [ $# -ne 3 ]
then
    echo "Incorrect arguments, arguments should looks like this!"
    echo "./backupd.sh source-dir backup-dir max-backups"
    exit 1
fi

# Get arguments
source_dir=$1
backup_dir=$2
max_backups=$3

# Validate the correctness of the arguments
if [ $max_backups -lt 1 ]
then
    echo "max backups should be positive integer!"
    exit 1
fi

if [ ! -d $source_dir ]
then
    echo "Invaild source directory!"
    exit 1
fi

if [ ! -d $backup_dir ]
then
    echo "Invaild backup directory!"
    exit 1
fi

#*********** Backup Code ***********#
last_dir_info=$backup_dir/directory-info.last
new_dir_info=$backup_dir/directory-info.new

ls -lR $source_dir > $new_dir_info # Get current file info

# If matched skip backup cycle
if cmp -s $last_dir_info $new_dir_info;
then
    exit 0
fi 

# Check number of backups
backup_number=$(ls $backup_dir -l | grep ^d | wc -l) # Count each time incase someone deleted folders

if [ $((backup_number)) -eq $max_backups ]
then
    old_backup=$(find $backup_dir/* -type d -print -quit) # Get oldest folder
    rm -rf $old_backup
fi

ls -lR $source_dir > $last_dir_info # Save current file info
dist_dir=$backup_dir/$(date '+%Y-%m-%d-%H-%M-%S')
cp -R $source_dir $dist_dir