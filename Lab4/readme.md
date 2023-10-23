# Backup Script
**Backup Script** is a **shell script**  that constantly backup your work directory every sometime to another directory that the user choose and if you don't change any thing with your working direcotry the backup cycle will be skipped.

## Folder hieratically 
There is **3 main files**
- **backup-cron.sh**
- **backupd.sh**
- **makefile**
- **readme.md** <--- current file
 
**backup-cron.sh**: This file is the main script which contain the code of backup using cron.

**backupd.sh**: This file is the main script which contain the code of backup without cron.

**makefile**: This file is responsible of running the script correctly.

**readme.md**: For Documentation.

## Prerequisites
You must have those available in your system
- make
- cron (Optional)


### make 
First update by typing.
```
sudo apt update
```

Then you can check if you had make from this command
```
make -version
```
![make version output](make_version_out.png)

Install **make** by typing.
```
sudo apt install make
```

### Cron
If you are going to use cron you have to make sure that cron server is running by using the following command
```
sudo systemctl status cron.service
```

## Step by Step Instructions Without cron

You have to run the makefile using the following command
```
make SOURCE=? BACKUP=? SECS=? MAX=?
```

**SOURCE:** the directory you want to backup **(Default: ~/workspace)**.

**BACKUP:** backup directory **(Default: ~/work_backup)**. 

**SECS:** backup every how much time in sec **(Default: 1min)**.

**MAX:** max number of backups before delete oldest backup **(Default: 10)**.

This is how to run with default arguments
```
make
```
### Note
So you can run your code with those default values but make sure that you have **~/workspace** directory or you will get an **error!**.

## Step by Step Instructions using cron


### Add the script in the crontab
First open crontab file
```
crontab -e
```
And add this line on it and save.
```
*/1 * * * * /bin/sh (backup-cron.sh --> location) source_dir backup_dir max_backup
```

This line will backup your **source_dir** every **1 minute** to **backup_dir** and remove oldest backup after **max_backup**.

to change the time this website will help you **[Crontab guru]([https://](https://crontab.guru/))**

### Example
We want to backup every 3rd Friday of the month at 12:31AM the line will looks like this
```
31 0 ? * FRI#3 /bin/sh (backup-cron.sh --> location) source_dir backup_dir max_backup
```
