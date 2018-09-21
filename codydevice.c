#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/uaccess.h>
#include<linux/slab.h>

//MODULE_AUTHOR("Cody Hegwer")
//MODULE_LICENSE("GPL")

// file operations struct

ssize_t device_file_read( struct file *f, char *buff, size_t count, loff_t *offset_loc);
ssize_t device_file_write( struct file *f, const char *buff, size_t count, loff_t *offset_loc);
loff_t device_file_seek( struct file *f, loff_t offset, int seek_start );
static int device_file_open( struct inode *i, struct file *f );
static int device_file_close( struct inode *i, struct file *f );
int count_available_bytes( int offset );

static struct file_operations simple_driver_fops = {
	.owner = THIS_MODULE,
	.read = device_file_read,
	.write = device_file_write,
	.llseek = device_file_seek,
	.open = device_file_open,
	.release = device_file_close,
};

// global variables

static char *DRIVER_BUFF;
static int ok_buff[100] = {0}; // an array instantiated to 0's, 1 indicates ok to copy to user.
static int device_file_major_number = 240;
static const char device_name[] = "Simple_Device";


// register device

static int register_device(void){
	
	int result = 0;
	printk( KERN_NOTICE "Simple-driver: register_device() is called." );
	result = register_chrdev( 240 , device_name, &simple_driver_fops );

	if( result < 0){
		printk( KERN_WARNING "Simple-driver: can\'t register character device with errorcode = %i", result );
		return result;
	}
	
	// allocate memory for driver buffer
	DRIVER_BUFF = kmalloc(100, GFP_KERNEL);

	device_file_major_number = result;
	printk( KERN_NOTICE "Simple-driver: registered character device with major number = %i and minor numbers 0...255", device_file_major_number );
	return 0;
}

void exit_mod(void){
	unregister_chrdev( 240 , device_name );
	kfree(DRIVER_BUFF);
	printk(KERN_ALERT "Simple-driver: exited cody's module.");
}




ssize_t device_file_read( struct file *f, char *buff, size_t count, loff_t *offset_loc ){
	
	int errno, access, available_bytes;

	printk( KERN_NOTICE "Simple-driver: driver is attempting to read; %d bytes.", count ); 
	
	available_bytes = count_available_bytes( *offset_loc );

	printk( KERN_NOTICE "Simple-driver: there are %d available bytes to read with offset at %lld.", available_bytes, *offset_loc );
 
	// are there any bytes at the offset location
	if ( available_bytes != 0 ) {
		
		// check if count is larger then what we have available to write.
		if ( count > available_bytes ){
			count = available_bytes;
		}
		
		printk( KERN_NOTICE "Simple-driver: offset is at %lld, count to read is %d.", *offset_loc, count);
		
		// check if user buff is ok to write to.
		access = access_ok(VERIFY_WRITE, buff, count);
		if( access == 0 ){
			printk( KERN_ALERT "Simple-driver: they doing bad stuff bruh.");
			return 0;
		}

		errno = copy_to_user( buff, DRIVER_BUFF + *offset_loc, count );
			
		if ( errno == 0 ){
			printk( KERN_NOTICE "Simple-driver: driver successfully read.");
			
			// change offset pointer to end of whatever was just read.
			*offset_loc += count;
			
			return count;
		}
	}

	printk( KERN_NOTICE "Simple-driver: nothing left to read.");
	return 0;

} 




ssize_t device_file_write( struct file *f, const char *buff, size_t count, loff_t *offset_loc ){
	
	int errno, access, i;
	
	printk( KERN_NOTICE "Simple-driver: driver is attempting to write.");
	
	// only write the characters you can fit
	if ( count > 100 - *offset_loc ) {
		count = 100 - *offset_loc;
	}
	
	access = access_ok(VERIFY_READ, buff, count);

	if ( access == 0){
	     	printk( KERN_ALERT "Simple-driver: they trynna do bad stuff.");
		return 0;
	}

	errno = copy_from_user( DRIVER_BUFF + *offset_loc, buff, count );
		
	// adjust ok_buff to reflect change in Driver buffer
	for (i = *offset_loc; i < count + *offset_loc; i++){
		ok_buff[i] = 1;
	}

	// move offset pointer to end of whatever was just written
	//*offset_loc += count;
	
	if ( errno != 0 ){
		printk( KERN_ALERT "Simple-driver: driver failed to write.");
		return 0;
	}

	printk( KERN_NOTICE "Simple-driver: driver was able to write %d.", count);


	return count;
}




loff_t device_file_seek( struct file *f, loff_t offset, int seek_start ){
	
	loff_t new_offset_loc = 0;
	
	switch ( seek_start ){
	case 0: // SET
		new_offset_loc = offset;
		break;
	case 1: // CUR
		new_offset_loc = f->f_pos + offset;
		break;
	case 2: // END
		new_offset_loc = 99 - offset;
		break;
	}

	if ( new_offset_loc < 0 ) {
		new_offset_loc = 0;
	}

	if (new_offset_loc > 99 ) {
		new_offset_loc = 99;
	}

	// actually move the file offset to this new offset location
	f->f_pos = new_offset_loc;
	printk( KERN_ALERT "Simple-driver: offset is now %lld.", f->f_pos);
	return new_offset_loc;


}


static int device_file_open( struct inode *i, struct file *f){
	printk( KERN_NOTICE "Simple-driver: file opened" );
	return 0;
}

static int device_file_close( struct inode *i, struct file *f ){
	printk( KERN_NOTICE "Simple-driver: file closed" );
	return 0;
}

int count_available_bytes( int offset ){

	// count the available number of contiguous bytes from a given offset.
	int i;
	int count = 0;

	for ( i = offset; i < 100; i++){
		if ( ok_buff[i] == 0 ){
			break;
		}
		count++;
	}

	return count;
}




module_init(register_device);
module_exit(exit_mod);
