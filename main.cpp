#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    return a.exec();
}

//TODO
/*
 *Table 1: Path Table --> Content
 * PathName, Source, Destination, List of Conflicts
 * <-->PreProcessing
 *
 * Table 2: Time Table --> Content
 * Path Name, CarName, Time Taken
 * <-->PreProcessing
 *
 * Table 3: Movement Table --> Content
 * Car Type (Detected), Souce (Detected), Destination (Detected), PathName (R - T1), Priority Flag (R), TimeTaken (R - T2), Conflicts (R - T1), Wait Time (Calculated)
 * <-->Realtime



