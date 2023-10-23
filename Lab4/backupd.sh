: '
    File:        backupd.sh
    Author:      Abdullah Elsayed Ahmed
    ID:          7459
    Description: Automatically backup folder
'

#*********** Validate arguments ***********#
# Validate number of arguments
if [ $# -ne 4 ]
then
    echo "Incorrect arguments, arguments should looks like this!"
    echo "./backupd.sh source-dir backup-dir interval-secs max-backups"
    exit 1
fi

# Get arguments
source_dir=$1
backup_dir=$2
interval_secs=$3
max_backups=$4

# Validate the correctness of the arguments
if [ $interval_secs -lt 1 ]
then
    echo "interval in seconds should be positive integer!"
    exit 1
fi

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

while true
do
    sleep $interval_secs # Wait til next backup cycle
    
    # Check if user delete the source directory or the backup directory at the runtime
    if [ ! -d $source_dir ]
    then
        echo "Source directory has been deleted!"
        exit 2
    fi

    if [ ! -d $backup_dir ]
    then
        echo "backup directory has been deleted!"
        exit 2
    fi
    
    ls -lR $source_dir > $new_dir_info # Get current file info

    # If matched skip backup cycle
    if cmp -s $last_dir_info $new_dir_info;
    then
        continue
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

done