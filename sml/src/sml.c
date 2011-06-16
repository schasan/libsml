// Copyright 2011 Juri Glass, Mathias Runge, Nadim El Sayed 
// DAI-Labor, TU-Berlin
// 
// This file is part of libSML.
// 
// libSML is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// libSML is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with libSML.  If not, see <http://www.gnu.org/licenses/>.


#include <sml/sml.h>
#include <sml/sml_shared.h>
#include <sml/sml_message.h>
#include <sml/sml_number.h>
#include <sml/sml_time.h>

#include <stdio.h>
#include <string.h>

// EDL meter must provide at least 250 bytes as a receive buffer
#define SML_FILE_BUFFER_LENGTH 512

/*
void mc_sml_hexdump_rest(sml_buffer *buf) {
	int i;
    unsigned char *rest = mc_sml_buf_get_current_buf(buf);
    for (i = 0; i < buf->buffer_len - buf->cursor; i++) {
		printf("%02X ", (unsigned char) rest[i]);
		if ((i + 1) % 8 == 0) {
			printf("\n");
		}
	}
	printf("\n");
}
*/

void mc_sml_hexdump(unsigned char *buffer, size_t buffer_len) {
	printf("SML dump (%zu bytes)\n", buffer_len);
	hexdump(buffer, buffer_len);
}

sml_file *sml_file_parse(unsigned char *buffer, size_t buffer_len) {
    
	//buffer += 8;
	//buffer_len -= 16;
    sml_file *file = (sml_file*) malloc(sizeof(sml_file));
    memset(file, 0, sizeof(sml_file));
    
    sml_buffer *buf = sml_buffer_init(buffer_len);
    memcpy(buf->buffer, buffer, buffer_len);
    file->buf = buf;
    
    sml_message *msg;
    
    // parsing all messages
    for (; buf->cursor < buf->buffer_len;) {
        
        if(mc_sml_buf_get_current_byte(buf) == SML_MESSAGE_END) {
            // reading trailing zeroed bytes
            mc_sml_buf_update_read_bytes(buf, 1);
            continue;
        }
        
		msg = sml_message_parse(buf);
		
        if (mc_sml_buf_has_errors(buf)) {
			printf("warning: could not read the whole file\n");
           break;
        }

        sml_file_add_message(file, msg);

	}
    return file;
}

sml_file *sml_file_init() {
    sml_file *file = (sml_file*) malloc(sizeof(sml_file));
    memset(file, 0, sizeof(sml_file));
    
    sml_buffer *buf = sml_buffer_init(SML_FILE_BUFFER_LENGTH);
    file->buf = buf;
    return file;
}

void sml_file_add_message(sml_file *file, sml_message *message) {
    file->messages_len++;
    file->messages = (sml_message **) realloc(file->messages, sizeof(sml_message *) * file->messages_len);
    file->messages[file->messages_len - 1] = message;
}

void sml_file_write(sml_file *file) {
   
    int i;
    if (file->messages && file->messages_len > 0) {
        for (i = 0; i < file->messages_len; i++) {
            sml_message_write(file->messages[i], file->buf);
        }
    }
}

void sml_file_free(sml_file *file) {
    if (file) {
        
        if (file->messages) {
            int i;
            for (i = 0; i < file->messages_len; i++) {
                sml_message_free(file->messages[i]);
            }
            free(file->messages);
        }
        
        if (file->buf) {
            sml_buffer_free(file->buf);
        }
        
        free(file);
    }
}

void sml_file_print(sml_file *file) {
    int i;
    printf("SML file (%d SML messages, %d bytes)\n", file->messages_len, file->buf->cursor);
    for (i = 0; i < file->messages_len; i++) {
        printf("SML message %4.X\n", file->messages[i]->message_body->tag);
    }
}










											 
											 


