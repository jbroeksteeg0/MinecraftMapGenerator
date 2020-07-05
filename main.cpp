#include <iostream>
#include <iterator>
#include <fstream>
#include <vector>
#include <cassert>
#include <stdio.h>
#include <bitset>
#include <map>
#include <climits>
// #include <zlib.h>
#include <cstring>
#include <set>
#include <experimental/filesystem>
#include <math.h>

#include "parser.h"
#define NBT_IMPLEMENTATION
#include "nbt.h"

#define SEGSIZE 16384
#define RADIUS 1
#define CACHE_SIZE 80
#define AFTER_1_15 0

using namespace std;

vector<unsigned char> buffer;
vector<int64_t> chunkLocations(1024), chunkSizes(1024);
map<vector<int>, string> currentMap;

static size_t reader_read(void* userdata, uint8_t* data, size_t size) {
  return fread(data, 1, size, userdata);
}
static void print_nbt_tree(nbt_tag_t* tag, int64_t indentation) {
  for (int64_t i = 0; i < indentation; i++) {
    printf(" ");
  }

  if (tag->name) {
    printf("tag name: %s: ", tag->name);
  }

  switch (tag->type) {
    case NBT_TYPE_END: {
      printf("[end]");
      break;
    }
    case NBT_TYPE_BYTE: {
      printf("byte: %hhd", tag->tag_byte.value);
      break;
    }
    case NBT_TYPE_SHORT: {
      printf("short: %hd", tag->tag_short.value);
      break;
    }
    case NBT_TYPE_INT: {
      printf("int: %d", tag->tag_int.value);
      break;
    }
    case NBT_TYPE_LONG: {
      printf("long: %ld", tag->tag_long.value);
      break;
    }
    case NBT_TYPE_FLOAT: {
      printf("%f", tag->tag_float.value);
      break;
    }
    case NBT_TYPE_DOUBLE: {
      printf("double: %f", tag->tag_double.value);
      break;
    }
    case NBT_TYPE_BYTE_ARRAY: {
      printf("[byte array]");
      for (size_t i = 0; i < tag->tag_byte_array.size; i++) {
        printf("%hhd ", tag->tag_byte_array.value[i]);
      }
      break;
    }
    case NBT_TYPE_STRING: {
      printf("string: %s", tag->tag_string.value);
      break;
    }
    case NBT_TYPE_LIST: {
      printf("list:\n ");
      for (size_t i = 0; i < tag->tag_list.size; i++) {
        print_nbt_tree(tag->tag_list.value[i], indentation + tag->name_size + 2);
      }
      break;
    }
    case NBT_TYPE_COMPOUND: {
      printf("compound:\n ");
      for (size_t i = 0; i < tag->tag_compound.size; i++) {
        print_nbt_tree(tag->tag_compound.value[i], indentation + tag->name_size + 2);
      }
      break;
    }
    case NBT_TYPE_INT_ARRAY: {
      printf("[int array]");
      break;
      for (size_t i = 0; i < tag->tag_int_array.size; i++) {
        printf("%d ", tag->tag_int_array.value[i]);
      }
      break;
    }
    case NBT_TYPE_LONG_ARRAY: {
      printf("[long array]");
      for (size_t i = 0; i < tag->tag_long_array.size; i++) {
        printf("%ld ", tag->tag_long_array.value[i]);
      }
      break;
    }
    default: {
      printf("[error]");
    }
  }

  printf("\n");
}

int64_t getChunkOffset(int64_t x, int64_t z) {
    return (x&31) + ((z&31) * 32);
}

int64_t  bitRange(int64_t value, int64_t from, int64_t to) {
  int64_t ret = 0;
  bitset<64> bs = value;

  for (int i = from; i <= to; i++) {
    ret |= bs[i];
  }
  return ret;
}

void getTop(int64_t chunkX, int64_t chunkZ) {
    int64_t offset = getChunkOffset(chunkX,chunkZ);
    if (chunkSizes[offset] == 0) {
        return;
    }

    int64_t curr = chunkLocations[offset] * 4096; // starting location of chunk

    int64_t length = 0;
    length += (int64_t)buffer[curr++] << (int64_t)24;
    length += (int64_t)buffer[curr++] << (int64_t)16;
    length += (int64_t)buffer[curr++] << (int64_t)8;
    length += (int64_t)buffer[curr++];

    int64_t compressionType = buffer[curr++];
    vector<unsigned char> compressed, uncompressed;

    if (compressionType != 2){
        cout << "WRONG COMPRESSION\n";
        return;
    }
    for (int64_t i = 0; i < length; i++) {
        compressed.push_back(buffer[curr++]);
    }
    ofstream("temp.bin",ios::binary).write(reinterpret_cast<const char*>(compressed.data()), compressed.size());

    nbt_tag_t*  rootTag;
    {
        FILE* f = fopen("temp.bin","rb");
        nbt_reader_t reader;
        reader.read = reader_read;
        reader.userdata = f;
        rootTag = nbt_parse(reader, NBT_PARSE_FLAG_USE_ZLIB);
        fclose(f);
    }


    bool found = false;
    rootTag = rootTag->tag_compound.value[0];

    for (int i = 0; i < rootTag->tag_compound.size; i++) {
      string re = "";

      for (int c = 0; c < rootTag->tag_compound.value[i]->name_size; c++) {
        re += rootTag->tag_compound.value[i]->name[c];
      }
      if (re == "Sections") {
        found=true;
        rootTag = rootTag->tag_compound.value[i];
        break;
      }
    }
    if (!found) {return;}

    map<vector<int>, string> blocks;
    map<vector<int>, string> heighest;
    multiset<pair<int,int>> toCheck;
    for (int x = 0; x < 16; x++) {
      for (int y = 0; y < 16; y++) {
        toCheck.insert({x,y});
      }
    }
    for (int64_t ymod = rootTag->tag_list.size-1; ymod >= 0 && toCheck.size(); ymod--) { // TODO might not have all Ys
        // if (ymod>4) {continue;}
        vector<int> blockStateInds;
        vector<string> paletteNames;
        nbt_tag_t* newRoot = rootTag->tag_list.value[ymod];
        if (newRoot->tag_compound.size>1e9){ // glitched
          continue;
        }
        vector<uint64_t> blockData;

        if (newRoot->type != NBT_TYPE_COMPOUND) {
          cout << "Skipping ymod " << ymod << endl;
          continue;
        }
        
        int foundY = -1;
        for (int64_t i = 0; i < newRoot->tag_compound.size; i++) {
            string reconstructedName = "";
            for (int64_t s = 0; s < newRoot->tag_compound.value[i]->name_size; s++) {
                reconstructedName += *(newRoot->tag_compound.value[i]->name + s);
            }
            if (reconstructedName == "BlockStates") {
                nbt_tag_t* stateParent = newRoot->tag_compound.value[i];
                
                for (int64_t j = 0; j < stateParent->tag_long_array.size; j++) {
                    blockData.push_back(stateParent->tag_long_array.value[j]);
                }
            } else if (reconstructedName == "Palette") {
                nbt_tag_t* stateParent = newRoot->tag_compound.value[i];
                for (int ind = 0; ind < stateParent->tag_list.size; ind++){
                    int compoundSize = stateParent->tag_list.value[ind]->tag_compound.size;
                    paletteNames.push_back(stateParent->tag_list.value[ind]->tag_compound.value[compoundSize-1]->tag_string.value); 
                }
            } else if (reconstructedName == "Y") {
                foundY = newRoot->tag_compound.value[i]->tag_byte.value;
            }

        }
        if (foundY == -1){continue;}
        if (blockData.size()){

            if (AFTER_1_15) {
                int failed = 0;
                int bits = 64;
                int thingsPerLong =(int) ceil((float)4096 / (float)blockData.size());
                thingsPerLong = min(thingsPerLong, 16);
                int perThing = bits / thingsPerLong; 

                for (uint64_t i: blockData) { // should work for other ones?
                    bitset<64> bs = i;
                    for (int j = 0; j < thingsPerLong; j++) {
                        int newVal = 0;
                        for (int k = 0; k < perThing; k++){
                            if (bs[j*perThing + k]) {
                                newVal |= 1<<k;
                            }
                        }
                        blockStateInds.push_back(newVal);
                    }

                }
            } else {
                int perThing = (64 * blockData.size()) / 4096;
                vector<bool> bits;
                for (uint64_t i: blockData) {
                    bitset<64> bs = i;
                    // cout << bs << "\n";
                    // cout << i << "\n";
                    for (int j = 0; j < 64; j++){
                        bits.push_back(bs[j]);
                    }
                }
                int cnt = 0;
                int current = 0;
                for (int i = 0; i < bits.size(); i++){
                    current |= bits[i] << cnt;
                    cnt++;
                    if (cnt == perThing) {
                        blockStateInds.push_back(current);
                        // cout << "got " << current << "\n";
                        cnt=0;
                        current=0;
                    }
                }
                // exit(0);
            }
        }

        if (blockStateInds.empty()) {continue;}

        for (int y = 15; y >= 0; y--) {
          vector<pair<int,int>> toRemove;
          for (pair<int,int> pi: toCheck) {
            int x = pi.first;
            int z = pi.second;
            int ind = x + (z*16) + (y*16*16);
            int worldX = (16*chunkX) + x;
            int worldZ = (16*chunkZ) + z;

            if (blockStateInds[ind] == -1){
                continue; // skipped, probably air or somethn
            }

            if (paletteNames.size()<=blockStateInds[ind])  {
                continue;
            }
            if (blockStateInds[x + (z*16) + (y*16*16)] != -1) {
                string name = paletteNames[blockStateInds[x + (y*(16*16)) + z*16]];
                if (!skipBlocks.count(name)) {
                    currentMap[{worldX, worldZ}] = paletteNames[blockStateInds[x + (z*16) + (y*(16*16))]];
                    toRemove.push_back(pi);
                }

            }
          }
          for (pair<int,int> pi: toRemove) {
            toCheck.erase(pi);
          }
        }

    }
    
    // cout << "success\n";
    nbt_free_tag(rootTag);
    // return heighest;
}

int main() {
    // get file names
    vector<pair<int,int>> regions;
    string dir = "TestWorld/";
    for (const auto& entry : std::experimental::filesystem::directory_iterator(dir)) {
      const auto str = entry.path().filename().string();

      if (str.size() < 3 || !(str[str.size() - 1] == 'a' && str[str.size() - 2] == 'c' && str[str.size() - 3] == 'm')) {
        continue;
      }
      stringstream ss(str);
      char dumb;
      string currentNumber = "";
      int numOne, numTwo;
      while (true) {
        ss >> dumb;
        if (dumb == '.') {
          break;
        }
      }
      while (true) {
        ss >> dumb;
        if (dumb == '.'){
          break;
        }
        currentNumber += dumb;
      }
      numOne = stoi(currentNumber);
      currentNumber = "";
      while (true) {
        ss >> dumb;
        if (dumb == '.'){
          break;
        }
        currentNumber += dumb;
      }
      numTwo = stoi(currentNumber);

      if (numOne >= -RADIUS && numOne <= RADIUS && numTwo >= -RADIUS && numTwo <= RADIUS) {
          regions.push_back({numOne, numTwo});
      }
    }

    startFile(RADIUS);

    int c = 1;
    for (pair<int,int> currRegion: regions) {
        int regionX = currRegion.first;
        int regionZ = currRegion.second;
        if (max(regionX,regionZ) > RADIUS || min(regionX,regionZ) < -RADIUS) {continue;}
        string regionName = dir+"r." + to_string(currRegion.first) + "." + to_string(currRegion.second) + ".mca";
        cout << "Processing " << c << "/" << regions.size() << ": " << regionName << "\n";
        c++;
        ifstream inputStream(regionName, ios::binary);

        buffer.clear(); buffer.resize(1024);
        chunkLocations.clear(); chunkLocations.resize(1024);
        chunkSizes.clear(); chunkSizes.resize(1024);

        buffer = vector<unsigned char>(istreambuf_iterator<char>(inputStream), {});
        if (buffer.size()<1024*4){continue;}
        int64_t curr = 0; 
        for (int64_t i = 0; i < 1024; i++) {
            // each location stamp
            // first 3 bytes are offset
            // last byte is length of chunk?
            chunkLocations[i] += ((int64_t)buffer[curr++]) << 16;
            chunkLocations[i] += ((int64_t)buffer[curr++]) << 8;
            chunkLocations[i] += ((int64_t)buffer[curr++]);

            chunkSizes[i] += (int64_t)buffer[curr++];
        }

        int minX = regionX * 32;
        int minZ = regionZ * 32;

        int maxX = ((regionX+1) * 32) - 1;
        int maxZ = ((regionZ+1) * 32) - 1;
        bool found = false;

        for (int64_t i = minX; i <= maxX; i++) {
            for (int64_t j = minZ; j <= maxZ; j++) {
                getTop(i,j);
            }
        }
        if (c%CACHE_SIZE == 0){
            writeData(currentMap);
        }
    }
    writeData(currentMap);
    finishProcess();
    cout << "FINISHED\n";

    return 0;
}
