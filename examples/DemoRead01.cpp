/*
 * DemoRead01.cpp - small read demo using the E57 format Foundation API.
 *
 * Copyright (c) 2009 - 2010 Kevin Ackley (kackley@gwi.net)
 * Copyright (c) 2020 - 2022 Michele Adduci (adduci@tutanota.com)
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#include <iostream>
#include <openE57/openE57.h>
using namespace e57;
using namespace std;

void printSomePoints(ImageFile imf, CompressedVectorNode points);

int main(int /*argc*/, char** /*argv*/)
{
  try
  {
    /// Read file from disk
    ImageFile     imf("foo.e57", "r");
    StructureNode root = imf.root();

    /// Make sure vector of scans is defined and of expected type.
    /// If "/data3D" wasn't defined, the call to root.get below would raise an exception.
    if (!root.isDefined("/data3D"))
    {
      cout << "File doesn't contain 3D images" << endl;
      return 0;
    }
    Node n = root.get("/data3D");
    if (n.type() != E57_VECTOR)
    {
      cout << "bad file" << endl;
      return 0;
    }

    /// The node is a vector so we can safely get a VectorNode handle to it.
    /// If n was not a VectorNode, this would raise an exception.
    VectorNode data3D(n);

    /// Print number of children of data3D.  This is the number of scans in file.
    int64_t scanCount = data3D.childCount();
    cout << "Number of scans in file:" << scanCount << endl;

    /// For each scan, print out first 4 points in either Cartesian or Spherical coordinates.
    for (int scanIndex = 0; scanIndex < scanCount; scanIndex++)
    {
      /// Get scan from "/data3D", assume its a Structure (else get exception)
      StructureNode scan(data3D.get(scanIndex));
      cout << "got:" << scan.pathName() << endl;

      /// Get "points" field in scan.  Should be a CompressedVectorNode.
      CompressedVectorNode points(scan.get("points"));
      cout << "got:" << points.pathName() << endl;

      /// Call subroutine in this file to print the points
      printSomePoints(imf, points);
    }

    imf.close();
  }
  catch (E57Exception& ex)
  {
    ex.report(__FILE__, __LINE__, __FUNCTION__);
    return -1;
  }
  catch (std::exception& ex)
  {
    cerr << "Got an std::exception, what=" << ex.what() << endl;
    return -1;
  }
  catch (...)
  {
    cerr << "Got an unknown exception" << endl;
    return -1;
  }
  return 0;
}

void printSomePoints(ImageFile imf, CompressedVectorNode points)
{
  /// Need to figure out if has Cartesian or spherical coordinate system.
  /// Interrogate the CompressedVector's prototype of its records.
  StructureNode proto(points.prototype());

  /// The prototype should have a field named either "cartesianX" or "sphericalRange".
  if (proto.isDefined("cartesianX") && proto.isDefined("cartesianY") && proto.isDefined("cartesianZ"))
  {
#if 1 //??? pick one?
      /// Make a list of buffers to receive the xyz values.
    const int                N = 4;
    vector<SourceDestBuffer> destBuffers;
    double                   x[N];
    destBuffers.push_back(SourceDestBuffer(imf, "cartesianX", x, N, true));
    double y[N];
    destBuffers.push_back(SourceDestBuffer(imf, "cartesianY", y, N, true));
    double z[N];
    destBuffers.push_back(SourceDestBuffer(imf, "cartesianZ", z, N, true));

    /// Create a reader of the points CompressedVector, try to read first block of N points
    /// Each call to reader.read() will fill the xyz buffers until the points are exhausted.
    CompressedVectorReader reader   = points.reader(destBuffers);
    unsigned               gotCount = reader.read();
    cout << "  got first " << gotCount << " points" << endl;

    /// Print the coordinates we got
    for (unsigned i = 0; i < gotCount; i++)
      cout << "  " << i << ". x=" << x[i] << " y=" << y[i] << " z=" << z[i] << endl;
#else
    /// Make a list of buffers to receive the xyz values.
    vector<SourceDestBuffer> destBuffers;
    int64_t                  columnIndex[10];
    destBuffers.push_back(SourceDestBuffer(imf, "columnIndex", columnIndex, 10, true));

    /// Create a reader of the points CompressedVector, try to read first block of 4 columnIndex
    /// Each call to reader.read() will fill the xyz buffers until the points are exhausted.
    CompressedVectorReader reader   = points.reader(destBuffers);
    unsigned               gotCount = reader.read();
    cout << "  got first " << gotCount << " points" << endl;

    /// Print the coordinates we got
    for (unsigned i = 0; i < gotCount; i++)
      cout << "  " << i << ". columnIndex=" << columnIndex[i] << endl;
#endif
    reader.close();
  }
  else if (proto.isDefined("sphericalRange"))
  {
    //??? not implemented yet
  }
  else
    cout << "Error: couldn't find either Cartesian or spherical points in scan" << endl;
}
