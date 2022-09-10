## Feature goals:
- provide an execuatable target with network interface
- and a library api

## TODO:
#### Now:
- Add tests
- Read in DB from file
- Recover DB from log

#### Later:
- Doxygen

# Diagrams
### ClassDiagram:
``` mermaid
%%{
  init: {'theme': 'dark'}
}%%

classDiagram
    class LSM {
    }

    class SST {
      - addSegment(memtable)
      + write(mem_table)
      + get(key)
    }

    class Segment {
      + new Segment(memtable)
      + bool contains(key)
      + option getValue(key)
    }

    DB --> LSM 
    DB "1" --> "*" Client 
    Client --> Socket
    TableFile --|> File
    LSM --> SST
    LSM --> Log 
    LSM --> MemTable 
    Segment --> Filter 
    Segment --> Sparse Index
    SST "1" --> "*" Segment
    SST "1" -- "1" TableFile
    TableFile "1" --o "*" Segment
    Sparse Index --> TableFile
    MemTable --|> map
    Filter --|> CBF
    Filter --|> Cuckoofilter
    MemTable --> Segment: "Flushes to a"
```
