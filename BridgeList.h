#ifndef BRIDGELIST_H_
#define BRIDGELIST_H_
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>



#include <iostream>

template<class DataType>
class Bridge {
public:
	unsigned int Bridge_Align;
	DataType* Bridge_Pointer;
	Bridge(int alignment,DataType* pointer){
		Bridge_Align = alignment;
		Bridge_Pointer = pointer;
	}
};



template<class DataType>
class BridgeList {
public:
	//int s = 0;
	unsigned int expand_size = 4;
	unsigned int virtual_size = 0;
	DataType* next_a;
	unsigned int Block_P = 0;
	unsigned int Block_PA = 1;
	unsigned int BridgeL_size = 1;
	Bridge<DataType>* BridgeL;

	BridgeList(){
		DataType* linkt = (DataType*) malloc (sizeof(DataType) * expand_size);
		BridgeL = (Bridge<DataType>*) malloc (sizeof(Bridge<DataType>) * 2);
		new (&BridgeL[0]) Bridge<DataType>(0, linkt);
		new (&BridgeL[1]) Bridge<DataType>(expand_size, nullptr);
		next_a = linkt;

	};

	~BridgeList(){
		next_a = nullptr;
		for(unsigned int location = 0; location < virtual_size; location++){
			this->get(location).~DataType();
		}
		for(unsigned int z=0;z< BridgeL_size;z++){
			free(BridgeL[z].Bridge_Pointer);
		}
		free(BridgeL);
	}

	//add item to end of list
	void add(const DataType& item);

	template <class object_type>
	void add_object();

	void add_sub_function();

	//replace item in list
	void replace(int location, DataType item);

	//remove last item from list
	void pop();

	//merge the whole list into one memory block segment
	void merge();

	//clears all stored values
	void clear();

	//extend and add item to the end of array
	Bridge<DataType>* extendA(Bridge<DataType>* array){
		BridgeL_size++;
		(Bridge<DataType>*) _expand(array,sizeof(Bridge<DataType>)*(BridgeL_size+1));
		if(errno == 12){
			Bridge<DataType>* temp = (Bridge<DataType>*) malloc (sizeof(Bridge<DataType>)*(BridgeL_size+1));
			for(unsigned int i =0;i<BridgeL_size+(unsigned int)1 ;i++){
				temp[i] = array[i];
			}
			free(array);
			return temp;
		}
		return array;

	}

	//extend and add item to the end of array
	void retractA(Bridge<DataType>* array){
		BridgeL_size--;
		(Bridge<DataType>*) _expand(array,sizeof(Bridge<DataType>)*(BridgeL_size+1));
		return;
	}
	//extend and add item to the end of array
	void clearA(Bridge<DataType>* array){
		BridgeL_size = 1;
		(Bridge<DataType>*) _expand(array,sizeof(Bridge<DataType>)*2);
		return;
	}


	//return size
	unsigned int size(){
		return virtual_size;
	}
	//return back
	DataType& back(){
		return operator[](virtual_size-1);
	}
	//get
	DataType& get(unsigned int i){
		return operator[](i);
	}
	//overload access to fit structure
	DataType& operator[](unsigned int i){
		while(i >= BridgeL[Block_P+1].Bridge_Align){
			Block_P++;
		}
		while(i < BridgeL[Block_P].Bridge_Align){
			Block_P--;
		}
		return (BridgeL[Block_P].Bridge_Pointer)[(i-BridgeL[Block_P].Bridge_Align)];
	}

	void* get_object(unsigned int i){
		void* object = &(operator[](i));
		return object;
	}

};
/**
 *  add() provides an interface to add an item to the end of bridgelist.
 *  if virtual_size exceeds actual_size then it doubles the bridgelist size.
 */
	template <class DataType>
	void BridgeList<DataType>::add(const DataType& item){
		add_sub_function();
		*next_a = item;
		virtual_size++;
		next_a++;
		return;
	}
/**
 * add_object() provides an interface to add a initialized object to the end of bridgelist.
 * if virtual_size exceeds actual_size then it doubles the bridgelist size.
 *
 */
	template <class DataType>
	template <class object_type>
	void BridgeList<DataType>::add_object(){
			add_sub_function();
			new (next_a) object_type;
			virtual_size++;
			next_a++;
			return;
		}

	template <class DataType>
	void BridgeList<DataType>::add_sub_function(){
		if(BridgeL[Block_PA].Bridge_Align <= virtual_size){
			if(Block_PA < BridgeL_size){
				Block_PA++;
				next_a = BridgeL[Block_PA-1].Bridge_Pointer;
			}
			else{
				(DataType*) _expand(BridgeL[BridgeL_size - 1].Bridge_Pointer, sizeof(DataType)*((16) + BridgeL[BridgeL_size].Bridge_Align - BridgeL[BridgeL_size - 1].Bridge_Align));
				if(errno == 0){
					BridgeL[BridgeL_size].Bridge_Align += 16;
					//s++;
				}
				if(errno == 12){
					DataType* linkt = (DataType*) malloc (sizeof(DataType) * expand_size);
					BridgeL[BridgeL_size].Bridge_Pointer = linkt;
					BridgeL = extendA(BridgeL);
					new (&BridgeL[BridgeL_size]) Bridge<DataType>(virtual_size + expand_size, nullptr);
					next_a = linkt;
					expand_size *= 2;
					Block_PA++;
				}
				_set_errno(0);
			}
		}
		return;
	}
/**
 * replace() changes a given entry to the new given item
 */
	template <class DataType>
	void BridgeList<DataType>::replace(int location, DataType item){
		this->operator[](location) = item;
			return;
	}

/**
 * pop() removes the last item in the BridgeList
 * if removing an item puts the size at 1/3 max capacity then the bridgeList will half its size
 */
	template <class DataType>
	void  BridgeList<DataType>::pop(){
		this->back().~DataType();
		virtual_size--;
		if(virtual_size < BridgeL[Block_PA - 1].Bridge_Align){
			Block_PA--;
			next_a = BridgeL[Block_PA - 1].Bridge_Pointer;
			next_a += BridgeL[Block_PA].Bridge_Align - BridgeL[Block_PA - 1].Bridge_Align - 1;
		}
		else{
			next_a--;
		}
		if(virtual_size == ceil(BridgeL[BridgeL_size - 1].Bridge_Align * .6)){
			if(BridgeL_size == 1){
				return;
			}
			expand_size /= 2;
			free (BridgeL[BridgeL_size - 1].Bridge_Pointer);
			if(Block_P == BridgeL_size - 1){
				retractA(BridgeL);
				Block_P = BridgeL_size - 1;
			}
			else{
				retractA(BridgeL);
			}

		}
		return;
	}
/**
 * merges the Bridges list into one continuous memory segment
 * Useful for applications that have a lot of variables and need fast read/write speeds
 */
	template <class DataType>
	void  BridgeList<DataType>::merge(){
		DataType* linkt = (DataType*) malloc (sizeof(DataType) * BridgeL[BridgeL_size].Bridge_Align);
		int i;
		for(i = 0; i < virtual_size; i++){
			linkt[i] = this->operator[](i);
		}
		next_a = &linkt[i];
		Block_P = 0;
		//reset vars
		for(unsigned int z = 0; z< (BridgeL_size); z++){
			free (BridgeL[z].Bridge_Pointer);
		}
		clearA(BridgeL);
		BridgeL[0] = Bridge<DataType>(0, linkt);
		BridgeL[1] = Bridge<DataType>(expand_size, nullptr);
	}
/**
 * clears the BridgeList of all values
 * leaves the BridgeList with a size of 5
 */

//make sure to call deconstructor of datatype
	template <class DataType>
	void BridgeList<DataType>::clear(){
		for(unsigned int location = 0; location < virtual_size; location++){
			this->get(location).~DataType();
		}
		for(unsigned int z = 0; z < (BridgeL_size); z++){
			free (BridgeL[z].Bridge_Pointer);
		}
		clearA(BridgeL);
//		s = 0;
		expand_size = 5;
		virtual_size = 0;
		DataType* linkt = (DataType*) malloc (sizeof(DataType) * expand_size);
		BridgeL[0] = Bridge<DataType>(0, linkt);
		BridgeL[1] = Bridge<DataType>(expand_size, nullptr);
		next_a = linkt;
		Block_P = 0;
		Block_PA = 1;
		BridgeL_size = 1;
	}





#endif /* BRIDGELIST_H_ */
