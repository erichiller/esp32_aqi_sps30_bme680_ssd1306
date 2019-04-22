typedef struct SensorValueSPS30 {
    int id;
    char title[256];
    char summary[2048];
    int numberOfAuthors;
    struct Author *authors;
};