/*******************************
 * connect.cpp
 *
 *  Created on: April, 2020
 *  Author    : keith
 *
 ********************************/

#include "bearldc.h"
#include "connect.h"


/********************* OOB Connection state linked list routines ***************************************/


/********************* Connection List linked list rountines ***************************************/

/* Add a comm thread to the list handler list.  */
pError
CCL::AddConnection(unsigned int ip, char *DevID)
{
	struct connect_struct *ptr = head;
	struct connect_struct *temp;

	pthread_mutex_lock(&lock1);

	try {

		temp = (connect_struct *)malloc(sizeof(connect_struct));

		if (temp == NULL)
		{
			return MALLOC_ERROR;
		}

		temp->ip = ip;
		strcpy_s(temp->DevID, 640, DevID);
		time(&temp->stime);
		temp->next = NULL;

		while (ptr != NULL)
		{
			if (ptr->next == NULL)
			{
				pcnt++;
				ptr->next = temp;
				break;
			}
			ptr = ptr->next;
		}
	}
	catch (...)
	{
		pthread_mutex_unlock(&lock1);
		return EXCEPTION;
	}



	pthread_mutex_unlock(&lock1);
	return STATUS_OK;
}

/* Remove a connection node from list. */
pError
CCL::RemoveConnection(unsigned int ip)
{

	pthread_mutex_lock(&lock1);
	struct connect_struct *temp = head, *prev = NULL;
	try {

		// If head node itself holds the key to be deleted 
		if (temp != NULL && temp->ip == ip)
		{
			head = temp->next;   // Changed head 
			free(temp);               // free old head 
			ccnt--;
			return STATUS_OK;
		}

		// Search for the key to be deleted, keep track of the 
		// previous node as we need to change 'prev->next' 
		while (temp != NULL && temp->ip != ip)
		{
			prev = temp;
			temp = temp->next;
		}

		// If key was not present in linked list 
		if (temp == NULL)
		{
			return STATUS_OK;
		}

		// Unlink the node from linked list 
		prev->next = temp->next;

		free(temp);  // Free memory 
		pcnt--;
	}
	catch (...)
	{
		pthread_mutex_unlock(&lock1);
		return EXCEPTION;
	}

	pthread_mutex_unlock(&lock1);
	return STATUS_OK;
}

/* Find ip in connection list. */
struct connect_struct *
CCL::FindIP(unsigned int ip)
{
	connect_struct *ptr = head;

	try {

		while (ptr != NULL)
		{
			if (ptr->ip == ip)
			{
				return ptr;
			}
			ptr = ptr->next;
		}
	}
	catch (...)
	{

	}

	return NULL;
}

/* Find DevID is in connection list. */
struct connect_struct *
CCL::FindID(char *DevID)
{
	connect_struct *ptr = head;

	try {

		while (ptr != NULL)
		{
			if (!memcmp(ptr->DevID, DevID, 640))
			{
				return ptr;
			}
			ptr = ptr->next;
		}
	}
	catch (...)
	{

	}

	return NULL;
}
/* End of internal connection state routines */


/********************* Socket Thread linked list rountines ***************************************/

/* Add a comm thread to the list handler list.  */
pError
CCL::AddThread(pthread_t ThreadID)
{
	struct connect_threads *ptr = thead;
	struct connect_threads *temp;

	pthread_mutex_lock(&lock1);

	try {

		temp = (connect_threads *)malloc(sizeof(connect_threads));

		if (temp == NULL)
		{
			return MALLOC_ERROR;
		}

		temp->ThreadID = ThreadID;
		time(&temp->stime);
		temp->next = NULL;

		while (ptr != NULL)
		{
			if (ptr->next == NULL)
			{
				pcnt++;
				ptr->next = temp;
				break;
			}
			ptr = ptr->next;
		}
	}
	catch (...)
	{
		pthread_mutex_unlock(&lock1);
		return EXCEPTION;
	}



	pthread_mutex_unlock(&lock1);
	return STATUS_OK;
}

/* Remove a connection node from list. */
pError
CCL::RemoveThread(pthread_t ThreadID)
{

	pthread_mutex_lock(&lock1);
	struct connect_threads *temp = thead, *prev = NULL;
	try {

		// If head node itself holds the key to be deleted 
		if (temp != NULL && temp->ThreadID.p == ThreadID.p)
		{
			thead = temp->next;   // Changed head 
			free(temp);               // free old head 
			pcnt--;
			return STATUS_OK;
		}

		// Search for the key to be deleted, keep track of the 
		// previous node as we need to change 'prev->next' 
		while (temp != NULL && temp->ThreadID.p != ThreadID.p)
		{
			prev = temp;
			temp = temp->next;
		}

		// If key was not present in linked list 
		if (temp == NULL)
		{
			return STATUS_OK;
		}

		// Unlink the node from linked list 
		prev->next = temp->next;

		free(temp);  // Free memory 
		pcnt--;
	}
	catch (...)
	{
		pthread_mutex_unlock(&lock1);
		return EXCEPTION;
	}

	pthread_mutex_unlock(&lock1);
	return STATUS_OK;
}

/* See if port is in ports list. */
struct connect_threads *
CCL::FindThread(pthread_t ThreadID)
{
	connect_threads *ptr = thead;

	try {

		while (ptr != NULL)
		{
			if (ptr->ThreadID.p == ThreadID.p)
			{
				return ptr;
			}
			ptr = ptr->next;
		}
	}
	catch (...)
	{

	}

	return NULL;
}
/* End of internal connection state routines */



/********************* Connection Thread linked list rountines ***************************************/


/* Add a comm thread to the list handler list.  */
pError
CCL::AddPort(unsigned short port, int Block)
{
	struct connect_ports *ptr = phead;
	struct connect_ports *temp;

	pthread_mutex_lock(&lock1);

	try {

		temp = (connect_ports *)malloc(sizeof(connect_ports));

		if (temp == NULL)
		{
			return MALLOC_ERROR;
		}

		temp->port= port;
		temp->Block = Block;
		temp->next = NULL;

		while (ptr != NULL)
		{
			if (ptr->next == NULL)
			{
				tcnt++;
				ptr->next = temp;
				break;
			}
			ptr = ptr->next;
		}
	}
	catch (...)
	{
		pthread_mutex_unlock(&lock1);
		return EXCEPTION;
	}



	pthread_mutex_unlock(&lock1);
	return STATUS_OK;
}

/* Remove a connection node from list. */
pError
CCL::RemovePort(unsigned short port)
{

	pthread_mutex_lock(&lock1);
	struct connect_ports *temp = phead, *prev = NULL;
	try {

		// If head node itself holds the key to be deleted 
		if (temp != NULL && temp->port == port)
		{
			phead = temp->next;   // Changed head 
			free(temp);               // free old head 
			tcnt--;
			return STATUS_OK;
		}

		// Search for the key to be deleted, keep track of the 
		// previous node as we need to change 'prev->next' 
		while (temp != NULL && temp->port != port)
		{
			prev = temp;
			temp = temp->next;
		}

		// If key was not present in linked list 
		if (temp == NULL)
		{
			return STATUS_OK;
		}

		// Unlink the node from linked list 
		prev->next = temp->next;

		free(temp);  // Free memory 
		tcnt--;
	}
	catch (...)
	{
		pthread_mutex_unlock(&lock1);
		return EXCEPTION;
	}

	pthread_mutex_unlock(&lock1);
	return STATUS_OK;
}

/* See if port is in ports list. */
struct connect_ports *
CCL::FindPort(unsigned short port)
{
	connect_ports *ptr = phead;

	try {

		while (ptr != NULL)
		{
			if (ptr->port == port)
			{
				return ptr;
			}
			ptr = ptr->next;
		}
	}
	catch (...)
	{

	}

	return NULL;
}
/* End of internal connection state routines */


