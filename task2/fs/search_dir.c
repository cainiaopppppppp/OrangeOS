#include "type.h"
#include "stdio.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "fs.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "keyboard.h"
#include "proto.h"
#include "hd.h"
#include "fs.h"

// PUBLIC int search_file(char * path)
// {
// 	int i, j;

// 	char filename[MAX_PATH];
// 	memset(filename, 0, MAX_FILENAME_LEN);
// 	struct inode * dir_inode;
// 	if (strip_path(filename, path, &dir_inode) != 0)
// 		return 0;

// 	if (filename[0] == 0)	/* path: "/" */
// 		return dir_inode->i_num;

// 	/**
// 	 * Search the dir for the file.
// 	 */
// 	int dir_blk0_nr = dir_inode->i_start_sect;
// 	int nr_dir_blks = (dir_inode->i_size + SECTOR_SIZE - 1) / SECTOR_SIZE;
// 	int nr_dir_entries =
// 	  dir_inode->i_size / DIR_ENTRY_SIZE; /**
// 					       * including unused slots
// 					       * (the file has been deleted
// 					       * but the slot is still there)
// 					       */
// 	int m = 0;
// 	struct dir_entry * pde;
// 	for (i = 0; i < nr_dir_blks; i++) {
// 		RD_SECT(dir_inode->i_dev, dir_blk0_nr + i);
// 		pde = (struct dir_entry *)fsbuf;
// 		for (j = 0; j < SECTOR_SIZE / DIR_ENTRY_SIZE; j++,pde++) {
// 			if (memcmp(filename, pde->name, MAX_FILENAME_LEN) == 0)
// 				return pde->inode_nr;
// 			if (++m > nr_dir_entries)
// 				break;
// 		}
// 		if (m > nr_dir_entries) /* all entries have been iterated */
// 			break;
// 	}

// 	/* file not found */
// 	return 0;
// }

PUBLIC int do_search_dir() {
	char* dir = fs_msg.Path;
	int pointer = 0;

	int i, j;

	char filename[MAX_PATH];
	memset(filename, 0, MAX_FILENAME_LEN);
	struct inode* dir_inode;
	if (strip_path(filename, dir, &dir_inode)) {
		return 0;
	}

	// if (filename[0] == 0) {
	// return dir_inode->i_num;
	// }
	/**
	 * Search the dir for the file.
	 */
	int dir_blk0_nr = dir_inode->i_start_sect;
	int nr_dir_blks = (dir_inode->i_size + SECTOR_SIZE - 1) / SECTOR_SIZE;
	int nr_dir_entries = dir_inode->i_size / DIR_ENTRY_SIZE; /**
															* including unused slots
															* (the file has been deleted
															* but the slot is still there)
															*/
	struct dir_entry* pde;
	for (i = 0; i < nr_dir_blks; i++) {
		RD_SECT(dir_inode->i_dev, dir_blk0_nr + i);
		pde = (struct dir_entry*)fsbuf;

		for (j = 0; j < SECTOR_SIZE / DIR_ENTRY_SIZE; j++, pde++) {
			dir[pointer] = ' ';
		pointer += 1;
		memcpy(dir + pointer, pde->name, strlen(pde->name));
		pointer += strlen(pde->name);
		}
	}
	return (void*)0;
 }