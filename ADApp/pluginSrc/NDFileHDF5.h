/* NDFileHDF5.h
 * Writes NDArrays to HDF5 files.
 *
 * Ulrik Kofoed Pedersen
 * March 20. 2011
 */
#ifndef NDFileHDF5_H
#define NDFileHDF5_H

#include <list>
#include <hdf5.h>
#include <asynDriver.h>
#include <NDPluginFile.h>
#include <NDArray.h>
#include "NDFileHDF5Layout.h"
#include "NDFileHDF5Dataset.h"
#include "NDFileHDF5LayoutXML.h"
#include "NDFileHDF5AttributeDataset.h"
#include "NDFileHDF5VersionCheck.h"

#define MAXEXTRADIMS 10

#define str_NDFileHDF5_nRowChunks        "HDF5_nRowChunks"
#define str_NDFileHDF5_nColChunks        "HDF5_nColChunks"
#define str_NDFileHDF5_nFramesChunks     "HDF5_nFramesChunks"
#define str_NDFileHDF5_chunkBoundaryAlign "HDF5_chunkBoundaryAlign"
#define str_NDFileHDF5_chunkBoundaryThreshold "HDF5_chunkBoundaryThreshold"
#define str_NDFileHDF5_NDAttributeChunk  "HDF5_NDAttributeChunk"
#define str_NDFileHDF5_nExtraDims        "HDF5_nExtraDims"
#define str_NDFileHDF5_extraDimOffsetX   "HDF5_extraDimOffsetX"
#define str_NDFileHDF5_extraDimOffsetY   "HDF5_extraDimOffsetY"
#define str_NDFileHDF5_storeAttributes   "HDF5_storeAttributes"
#define str_NDFileHDF5_storePerformance  "HDF5_storePerformance"
#define str_NDFileHDF5_totalRuntime      "HDF5_totalRuntime"
#define str_NDFileHDF5_totalIoSpeed      "HDF5_totalIoSpeed"
#define str_NDFileHDF5_flushNthFrame     "HDF5_flushNthFrame"
#define str_NDFileHDF5_compressionType   "HDF5_compressionType"
#define str_NDFileHDF5_nbitsPrecision    "HDF5_nbitsPrecision"
#define str_NDFileHDF5_nbitsOffset       "HDF5_nbitsOffset"
#define str_NDFileHDF5_szipNumPixels     "HDF5_szipNumPixels"
#define str_NDFileHDF5_zCompressLevel    "HDF5_zCompressLevel"
#define str_NDFileHDF5_dimAttDatasets    "HDF5_dimAttDatasets"
#define str_NDFileHDF5_layoutErrorMsg    "HDF5_layoutErrorMsg"
#define str_NDFileHDF5_layoutValid       "HDF5_layoutValid"
#define str_NDFileHDF5_layoutFilename    "HDF5_layoutFilename"
#define str_NDFileHDF5_posRunning        "HDF5_posRunning"
#define str_NDFileHDF5_posNameDimN       "HDF5_posNameDimN"
#define str_NDFileHDF5_posNameDimX       "HDF5_posNameDimX"
#define str_NDFileHDF5_posNameDimY       "HDF5_posNameDimY"
#define str_NDFileHDF5_posIndexDimN      "HDF5_posIndexDimN"
#define str_NDFileHDF5_posIndexDimX      "HDF5_posIndexDimX"
#define str_NDFileHDF5_posIndexDimY      "HDF5_posIndexDimY"
#define str_NDFileHDF5_fillValue         "HDF5_fillValue"
#define str_NDFileHDF5_SWMRCbCounter     "HDF5_SWMRCbCounter"
#define str_NDFileHDF5_SWMRSupported     "HDF5_SWMRSupported"
#define str_NDFileHDF5_SWMRMode          "HDF5_SWMRMode"
#define str_NDFileHDF5_SWMRRunning       "HDF5_SWMRRunning"

/** Writes NDArrays in the HDF5 file format; an XML file can control the structure of the HDF5 file.
  */
class epicsShareClass NDFileHDF5 : public NDPluginFile
{
  public:
    static const char *str_NDFileHDF5_extraDimSize[MAXEXTRADIMS];
    static const char *str_NDFileHDF5_extraDimName[MAXEXTRADIMS];
    static const char *str_NDFileHDF5_extraDimChunk[MAXEXTRADIMS];
    static const char *str_NDFileHDF5_posName[MAXEXTRADIMS];
    static const char *str_NDFileHDF5_posIndex[MAXEXTRADIMS];

    NDFileHDF5(const char *portName, int queueSize, int blockingCallbacks, 
               const char *NDArrayPort, int NDArrayAddr,
               int priority, int stackSize);
       
    /* The methods that this class implements */
    virtual asynStatus openFile(const char *fileName, NDFileOpenMode_t openMode, NDArray *pArray);
    virtual asynStatus readFile(NDArray **pArray);
    virtual asynStatus writeFile(NDArray *pArray);
    virtual asynStatus closeFile();
    virtual void report(FILE *fp, int details);
    virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
    virtual asynStatus writeOctet(asynUser *pasynUser, const char *value, size_t nChars, size_t *nActual);

    asynStatus startSWMR();
    asynStatus flushCallback();
    asynStatus createXMLFileLayout();
    asynStatus storeOnOpenAttributes();
    asynStatus storeOnCloseAttributes();
    asynStatus storeOnOpenCloseAttribute(hdf5::Element *element, bool open);
    asynStatus createTree(hdf5::Group* root, hid_t h5handle);
    asynStatus createHardLinks(hdf5::Group* root);

    hid_t writeHdfConstDataset( hid_t h5_handle, hdf5::Dataset* dset);
    hid_t writeH5dsetStr(hid_t element, const std::string &name, const std::string &str_value) const;
    hid_t writeH5dsetInt32(hid_t element, const std::string &name, const std::string &str_value) const;
    hid_t writeH5dsetFloat64(hid_t element, const std::string &name, const std::string &str_value) const;

    void writeHdfAttributes( hid_t h5_handle, hdf5::Element* element);
    hid_t createDataset(hid_t group, hdf5::Dataset *dset);
    void writeH5attrStr(hid_t element,
                          const std::string &attr_name,
                          const std::string &str_attr_value) const;
    void writeH5attrInt32(hid_t element, const std::string &attr_name, const std::string &str_attr_value) const;
    void writeH5attrFloat64(hid_t element, const std::string &attr_name, const std::string &str_attr_value) const;
    hid_t fromHdfToHidDatatype(hdf5::DataType_t in) const;
    hid_t createDatasetDetector(hid_t group, hdf5::Dataset *dset);

    int fileExists(char *filename);
    int verifyLayoutXMLFile();

    std::map<std::string, NDFileHDF5Dataset *> detDataMap;  // Map of handles to detector datasets, indexed by name
    std::map<std::string, hid_t>               attDataMap;  // Map of handles to attribute datasets, indexed by name
    std::string                                defDsetName; // Name of the default data set
    std::string                                ndDsetName;  // Name of NDAttribute that specifies the destination data set
    std::map<std::string, hdf5::Element *>     onOpenMap;   // Map of handles to elements with onOpen ndattributes, indexed by fullname
    std::map<std::string, hdf5::Element *>     onCloseMap;  // Map of handles to elements with onClose ndattributes, indexed by fullname

#ifndef _UNITTEST_HDF5_
  protected:
#endif
    /* plugin parameters */
    int NDFileHDF5_nRowChunks;
    #define FIRST_NDFILE_HDF5_PARAM NDFileHDF5_nRowChunks
    int NDFileHDF5_nColChunks;
    int NDFileHDF5_nFramesChunks;
    int NDFileHDF5_chunkBoundaryAlign;
    int NDFileHDF5_chunkBoundaryThreshold;
    int NDFileHDF5_NDAttributeChunk;
    int NDFileHDF5_nExtraDims;
    int NDFileHDF5_extraDimOffsetX;
    int NDFileHDF5_extraDimOffsetY;
    int NDFileHDF5_extraDimSize[MAXEXTRADIMS];
    int NDFileHDF5_extraDimName[MAXEXTRADIMS];
    int NDFileHDF5_extraDimChunk[MAXEXTRADIMS];
    int NDFileHDF5_storeAttributes;
    int NDFileHDF5_storePerformance;
    int NDFileHDF5_totalRuntime;
    int NDFileHDF5_totalIoSpeed;
    int NDFileHDF5_flushNthFrame;
    int NDFileHDF5_compressionType;
    int NDFileHDF5_nbitsPrecision;
    int NDFileHDF5_nbitsOffset;
    int NDFileHDF5_szipNumPixels;
    int NDFileHDF5_zCompressLevel;
    int NDFileHDF5_dimAttDatasets;
    int NDFileHDF5_layoutErrorMsg;
    int NDFileHDF5_layoutValid;
    int NDFileHDF5_layoutFilename;
    int NDFileHDF5_posRunning;
    int NDFileHDF5_posName[MAXEXTRADIMS];
    int NDFileHDF5_posIndex[MAXEXTRADIMS];
    int NDFileHDF5_fillValue;
    int NDFileHDF5_SWMRCbCounter;
    int NDFileHDF5_SWMRSupported;
    int NDFileHDF5_SWMRMode;
    int NDFileHDF5_SWMRRunning;

#ifndef _UNITTEST_HDF5_
  private:
#endif
    /* private helper functions */
    inline bool IsPrime(int number)
    {
      if (((!(number & 1)) && number != 2) || (number < 2) || (number % 3 == 0 && number != 3)){
        return false;
      }

      for(int k = 1; 36*k*k-12*k < number;++k){
        if ((number % (6*k+1) == 0) || (number % (6*k-1) == 0)){
          return false;
        }
      }
      return true;
    };

    hid_t typeNd2Hdf(NDDataType_t datatype);
    asynStatus configureDatasetDims(NDArray *pArray);
    asynStatus configureDims(NDArray *pArray);
    asynStatus configureCompression();
    char* getDimsReport();
    asynStatus writeStringAttribute(hid_t element, const char* attrName, const char* attrStrValue);
    asynStatus calculateAttributeChunking(int *chunking, int *mdim_chunking);
    asynStatus writeAttributeDataset(hdf5::When_t whenToSave, int positionMode, hsize_t *offsets);
    asynStatus closeAttributeDataset();
    asynStatus configurePerformanceDataset();
    asynStatus createPerformanceDataset();
    asynStatus writePerformanceDataset();
    void calcNumFrames();
    unsigned int calcIstorek();
    hsize_t calcChunkCacheBytes();
    hsize_t calcChunkCacheSlots();

    void checkForOpenFile();
    bool checkForSWMRMode();
    bool checkForSWMRSupported();
    void addDefaultAttributes(NDArray *pArray);
    asynStatus writeDefaultDatasetAttributes(NDArray *pArray);
    asynStatus createNewFile(const char *fileName);
    asynStatus createFileLayout(NDArray *pArray);
    asynStatus createAttributeDataset(NDArray *pArray);
    int isAttributeIndex(const std::string& attName);
    epicsInt32 findPositionIndex(NDArray *pArray, char *posName);


    hdf5::LayoutXML layout;

    int arrayDataId;
    int uniqueIdId;
    int timeStampId;
    int nextRecord;
    hid_t h5type;
    int *pAttributeId;
    NDAttributeList *pFileAttributes;
    epicsInt32 arrayDims[ND_ARRAY_MAX_DIMS];
    bool multiFrameFile;
    char *extraDimNameN;
    char *extraDimNameX;
    char *extraDimNameY;
    char *extraDimName[MAXEXTRADIMS];
    double *performanceBuf;
    double *performancePtr;
    epicsInt32 numPerformancePoints;
    epicsTimeStamp prevts;
    epicsTimeStamp opents;
    epicsTimeStamp firstFrame;
    double frameSize;  /** < frame size in megabits. For performance measurement. */
    int bytesPerElement;
    char *hostname;

    std::list<NDFileHDF5AttributeDataset*> attrList;

    /* HDF5 handles and references */
    hid_t file;
    hid_t dataspace;
    hid_t datatype;
    hid_t cparms;
    void *ptrFillValue;
    hid_t perf_dataset_id;

    /* dimension descriptors */
    int rank;               /** < number of dimensions */
    hsize_t *dims;          /** < Array of current dimension sizes. This updates as various dimensions grow. */
    hsize_t *maxdims;       /** < Array of maximum dimension sizes. The value -1 is HDF5 term for infinite. */
    hsize_t *chunkdims;     /** < Array of chunk size in each dimension. Only the dimensions that indicate the frame size (width, height) can really be tweaked. All other dimensions should be set to 1. */
    hsize_t *offset;        /** < Array of current offset in each dimension. The frame dimensions always have 0 offset but additional dimensions may grow as new frames are added. */
    hsize_t *framesize;     /** < The frame size in each dimension. Frame width, height and possibly depth (RGB) have real values -all other dimensions set to 1. */
    hsize_t *virtualdims;   /** < The desired sizes of the extra (virtual) dimensions: {Y, X, n} */
    char *ptrDimensionNames[ND_ARRAY_MAX_DIMS + MAXEXTRADIMS]; /** Array of strings with human readable names for each dimension */

    char *dimsreport;       /** < A string which contain a verbose report of all dimension sizes. The method getDimsReport fill in this */
};

#endif

