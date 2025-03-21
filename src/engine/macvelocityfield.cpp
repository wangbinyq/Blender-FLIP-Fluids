/*
MIT License

Copyright (C) 2025 Ryan L. Guy & Dennis Fassbaender

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "macvelocityfield.h"

#include "interpolation.h"
#include "threadutils.h"
#include "gridutils.h"

MACVelocityField::MACVelocityField() {
    _initializeVelocityGrids();
}

MACVelocityField::MACVelocityField(int isize, int jsize, int ksize, double dx) :
                                   _isize(isize), _jsize(jsize), _ksize(ksize),
                                   _dx(dx) {

    _initializeVelocityGrids();
}


MACVelocityField::~MACVelocityField() {
}

void MACVelocityField::_initializeVelocityGrids() {
    _u = Array3d<float>(_isize + 1, _jsize, _ksize, 0.0f);
    _v = Array3d<float>(_isize, _jsize + 1, _ksize, 0.0f);
    _w = Array3d<float>(_isize, _jsize, _ksize + 1, 0.0f);

    _u.setOutOfRangeValue(0.0f);
    _v.setOutOfRangeValue(0.0f);
    _w.setOutOfRangeValue(0.0f);
}

void MACVelocityField::getGridDimensions(int *i, int *j, int *k) {
    *i = _isize;
    *j = _jsize;
    *k = _ksize;
}

double MACVelocityField::getGridCellSize() {
    return _dx;
}

void MACVelocityField::setOutOfRangeVector(vmath::vec3 v) {
    _outOfRangeVector = v;
}

void MACVelocityField::clearU() {
    _u.fill(0.0);
}

void MACVelocityField::clearV() {
    _v.fill(0.0);
}

void MACVelocityField::clearW() {
    _w.fill(0.0);
}

void MACVelocityField::clear() {
    clearU();
    clearV();
    clearW();
}

Array3d<float>* MACVelocityField::getArray3dU() {
    return &_u;
}

Array3d<float>* MACVelocityField::getArray3dV() {
    return &_v;
}

Array3d<float>* MACVelocityField::getArray3dW() {
    return &_w;
}

float* MACVelocityField::getRawArrayU() {
    return _u.getRawArray();
}

float* MACVelocityField::getRawArrayV() {
    return _v.getRawArray();
}

float* MACVelocityField::getRawArrayW() {
    return _w.getRawArray();
}

float MACVelocityField::U(int i, int j, int k) {
    if (!isIndexInRangeU(i, j, k)) {
        return _outOfRangeVector.x;
    }

    return _u(i, j, k);
}

float MACVelocityField::V(int i, int j, int k) {
    if (!isIndexInRangeV(i, j, k)) {
        return _outOfRangeVector.y;
    }

    return _v(i, j, k);
}

float MACVelocityField::W(int i, int j, int k) {
    if (!isIndexInRangeW(i, j, k)) {
        return _outOfRangeVector.z;
    }

    return _w(i, j, k);
}

float MACVelocityField::U(GridIndex g) {
    if (!isIndexInRangeU(g)) {
        return _outOfRangeVector.x;
    }

    return _u(g);
}

float MACVelocityField::V(GridIndex g) {
    if (!isIndexInRangeV(g)) {
        return _outOfRangeVector.y;
    }

    return _v(g);
}

float MACVelocityField::W(GridIndex g) {
    if (!isIndexInRangeW(g)) {
        return _outOfRangeVector.z;
    }

    return _w(g);
}

void MACVelocityField::set(MACVelocityField &vfield) {
    int vi, vj, vk;
    vfield.getGridDimensions(&vi, &vj, &vk);
    FLUIDSIM_ASSERT(_isize == vi && _jsize == vj &&  _ksize == vk);

    for(int k = 0; k < _ksize; k++) {
        for(int j = 0; j < _jsize; j++) {
            for(int i = 0; i < _isize + 1; i++) {
                setU(i, j, k, vfield.U(i, j, k));
            }
        }
    }

    for(int k = 0; k < _ksize; k++) {
        for(int j = 0; j < _jsize + 1; j++) {
            for(int i = 0; i < _isize; i++) {
                setV(i, j, k, vfield.V(i, j, k));
            }
        }
    }

    for(int k = 0; k < _ksize + 1; k++) {
        for(int j = 0; j < _jsize; j++) { 
            for(int i = 0; i < _isize; i++) {
                setW(i, j, k, vfield.W(i, j, k));
            }
        }
    }
}

void MACVelocityField::setU(int i, int j, int k, double val) {
    if (!isIndexInRangeU(i, j, k)) {
        return;
    }

    _u.set(i, j, k, (float)val);
}

void MACVelocityField::setV(int i, int j, int k, double val) {
    if (!isIndexInRangeV(i, j, k)) {
        return;
    }

    _v.set(i, j, k, (float)val);
}

void MACVelocityField::setW(int i, int j, int k, double val) {
    if (!isIndexInRangeW(i, j, k)) {
        return;
    }

    _w.set(i, j, k, (float)val);
}

void MACVelocityField::setU(GridIndex g, double val) {
    setU(g.i, g.j, g.k, val);
}

void MACVelocityField::setV(GridIndex g, double val) {
    setV(g.i, g.j, g.k, val);
}

void MACVelocityField::setW(GridIndex g, double val) {
    setW(g.i, g.j, g.k, val);
}

void MACVelocityField::setU(Array3d<float> &ugrid) {
    FLUIDSIM_ASSERT(ugrid.width == _u.width && 
           ugrid.height == _u.height && 
           ugrid.depth == _u.depth);
    _u = ugrid;
}

void MACVelocityField::setV(Array3d<float> &vgrid) {
    FLUIDSIM_ASSERT(vgrid.width == _v.width && 
           vgrid.height == _v.height && 
           vgrid.depth == _v.depth);
    _v = vgrid;
}

void MACVelocityField::setW(Array3d<float> &wgrid) {
    FLUIDSIM_ASSERT(wgrid.width == _w.width && 
           wgrid.height == _w.height && 
           wgrid.depth == _w.depth);
    _w = wgrid;
}

void MACVelocityField::addU(int i, int j, int k, double val) {
    if (!isIndexInRangeU(i, j, k)) {
        return;
    }

    _u.add(i, j, k, (float)val);
}

void MACVelocityField::addV(int i, int j, int k, double val) {
    if (!isIndexInRangeV(i, j, k)) {
        return;
    }

    _v.add(i, j, k, (float)val);
}

void MACVelocityField::addW(int i, int j, int k, double val) {
    if (!isIndexInRangeW(i, j, k)) {
        return;
    }

    _w.add(i, j, k, (float)val);
}

void MACVelocityField::addU(GridIndex g, double val) {
    if (!isIndexInRangeU(g)) {
        return;
    }

    _u.add(g, (float)val);
}

void MACVelocityField::addV(GridIndex g, double val) {
    if (!isIndexInRangeV(g)) {
        return;
    }

    _v.add(g, (float)val);
}

void MACVelocityField::addW(GridIndex g, double val) {
    if (!isIndexInRangeW(g)) {
        return;
    }

    _w.add(g, (float)val);
}

vmath::vec3 MACVelocityField::velocityIndexToPositionU(int i, int j, int k) {
    FLUIDSIM_ASSERT(isIndexInRangeU(i, j, k));

    double gx = (double)(i-1)*_dx;
    double gy = (double)j*_dx;
    double gz = (double)k*_dx;

    return vmath::vec3(gx + _dx, gy + 0.5*_dx, gz + 0.5*_dx);
}

vmath::vec3 MACVelocityField::velocityIndexToPositionV(int i, int j, int k) {
    FLUIDSIM_ASSERT(isIndexInRangeV(i, j, k));

    double gx = (double)i*_dx;
    double gy = (double)(j-1)*_dx;
    double gz = (double)k*_dx;

    return vmath::vec3(gx + 0.5*_dx, gy + _dx, gz + 0.5*_dx);
}

vmath::vec3 MACVelocityField::velocityIndexToPositionW(int i, int j, int k) {
    FLUIDSIM_ASSERT(isIndexInRangeW(i, j, k));

    double gx = (float)i*_dx;
    double gy = (float)j*_dx;
    double gz = (float)(k-1)*_dx;

    return vmath::vec3(gx + 0.5f*_dx, gy + 0.5f*_dx, gz + _dx);
}

vmath::vec3 MACVelocityField::evaluateVelocityAtCellCenter(int i, int j, int k) {
    FLUIDSIM_ASSERT(Grid3d::isGridIndexInRange(i, j, k, _isize, _jsize, _ksize));

    double xavg = 0.5 * (U(i + 1, j, k) + U(i, j, k));
    double yavg = 0.5 * (V(i, j + 1, k) + V(i, j, k));
    double zavg = 0.5 * (W(i, j, k + 1) + W(i, j, k));

    return vmath::vec3(xavg, yavg, zavg);
}

float MACVelocityField::evaluateVelocityMagnitudeSquaredAtCellCenter(int i, int j, int k) {
    FLUIDSIM_ASSERT(Grid3d::isGridIndexInRange(i, j, k, _isize, _jsize, _ksize));

    double xavg = 0.5 * (U(i + 1, j, k) + U(i, j, k));
    double yavg = 0.5 * (V(i, j + 1, k) + V(i, j, k));
    double zavg = 0.5 * (W(i, j, k + 1) + W(i, j, k));

    return (float)(xavg*xavg + yavg*yavg + zavg*zavg);
}

float MACVelocityField::evaluateVelocityMagnitudeAtCellCenter(int i, int j, int k) {
    FLUIDSIM_ASSERT(Grid3d::isGridIndexInRange(i, j, k, _isize, _jsize, _ksize));

    double mag = evaluateVelocityMagnitudeSquaredAtCellCenter(i, j, k);
    if (mag > 0.0) {
        return (float)sqrt(mag);
    }
    else {
        return 0.0;
    }
}

float MACVelocityField::evaluateMaximumVelocityMagnitude() {
    double maxsq = 0.0;
    for (int k = 0; k < _ksize; k++) {
        for (int j = 0; j < _jsize; j++) {
            for (int i = 0; i < _isize; i++) {
                
                double m = evaluateVelocityMagnitudeSquaredAtCellCenter(i, j, k);
                maxsq = fmax(maxsq, m);
            }
        }
    }

    double max = maxsq;
    if (maxsq > 0.0) {
        max = sqrt(maxsq);
    }

    return (float)max;
}

vmath::vec3 MACVelocityField::evaluateVelocityAtFaceCenterU(int i, int j, int k) {
    FLUIDSIM_ASSERT(isIndexInRangeU(i, j, k));

    // Shift reference coordinate to the left. The formula used is for calculating
    // u(i+1/2, j, k). If we keep original (i,j,k) coordinate, then using the formula
    // would calculate u(i+3/2, j, k) instead. The same will be done for the V and W
    // faces, shifting back in the respective direction.
    i--;

    double vx = U(i+1, j, k);
    double vy = 0.25 * (V(i, j, k) + V(i, j+1, k) + V(i+1, j, k) + V(i+1, j+1, k));
    double vz = 0.25 * (W(i, j, k) + W(i, j, k+1) + W(i+1, j, k) + W(i+1, j, k+1));

    return vmath::vec3(vx, vy, vz);
}

vmath::vec3 MACVelocityField::evaluateVelocityAtFaceCenterV(int i, int j, int k) {
    FLUIDSIM_ASSERT(isIndexInRangeV(i, j, k));

    j--;

    double vx = 0.25 * (U(i, j, k) + U(i+1, j, k) + U(i, j+1, k) + U(i+1, j+1, k));
    double vy = V(i, j + 1, k);
    double vz = 0.25 * (W(i, j, k) + W(i, j, k+1) + W(i, j+1, k) + W(i, j+1, k+1));

    return vmath::vec3(vx, vy, vz);
}

vmath::vec3 MACVelocityField::evaluateVelocityAtFaceCenterW(int i, int j, int k) {
    FLUIDSIM_ASSERT(isIndexInRangeW(i, j, k));

    k--;

    double vx = 0.25 * (U(i, j, k) + U(i+1, j, k) + U(i, j, k+1) + U(i+1, j, k+1));
    double vy = 0.25 * (V(i, j, k) + V(i, j+1, k) + V(i, j, k+1) + V(i, j+1, k+1));
    double vz = W(i, j, k + 1);

    return vmath::vec3(vx, vy, vz);
}

double MACVelocityField::_interpolateU(double x, double y, double z) {
    if (!Grid3d::isPositionInGrid(x, y, z, _dx, _isize, _jsize, _ksize)) {
        return 0.0;
    }

    y -= 0.5*_dx;
    z -= 0.5*_dx;

    int i, j, k;
    double gx, gy, gz;
    Grid3d::positionToGridIndex(x, y, z, _dx, &i, &j, &k);
    Grid3d::GridIndexToPosition(i, j, k, _dx, &gx, &gy, &gz);

    double inv_dx = 1 / _dx;
    double ix = (x - gx)*inv_dx;
    double iy = (y - gy)*inv_dx;
    double iz = (z - gz)*inv_dx;

    int refi = i - 1;
    int refj = j - 1;
    int refk = k - 1;

    double points[4][4][4];
    for (int pk = 0; pk < 4; pk++) {
        for (int pj = 0; pj < 4; pj++) {
            for (int pi = 0; pi < 4; pi++) {
                points[pk][pj][pi] = U(pi + refi, pj + refj, pk + refk);
            }
        }
    }

    return Interpolation::tricubicInterpolate(points, ix, iy, iz);
}

double MACVelocityField::_interpolateV(double x, double y, double z) {
    if (!Grid3d::isPositionInGrid(x, y, z, _dx, _isize, _jsize, _ksize)) {
        return 0.0;
    }

    x -= 0.5*_dx;
    z -= 0.5*_dx;

    int i, j, k;
    double gx, gy, gz;
    Grid3d::positionToGridIndex(x, y, z, _dx, &i, &j, &k);
    Grid3d::GridIndexToPosition(i, j, k, _dx, &gx, &gy, &gz);

    double inv_dx = 1 / _dx;
    double ix = (x - gx)*inv_dx;
    double iy = (y - gy)*inv_dx;
    double iz = (z - gz)*inv_dx;

    int refi = i - 1;
    int refj = j - 1;
    int refk = k - 1;

    double points[4][4][4];
    for (int pk = 0; pk < 4; pk++) {
        for (int pj = 0; pj < 4; pj++) {
            for (int pi = 0; pi < 4; pi++) {
                points[pk][pj][pi] = V(pi + refi, pj + refj, pk + refk);
            }
        }
    }

    return Interpolation::tricubicInterpolate(points, ix, iy, iz);
}

double MACVelocityField::_interpolateW(double x, double y, double z) {
    if (!Grid3d::isPositionInGrid(x, y, z, _dx, _isize, _jsize, _ksize)) {
        return 0.0;
    }

    x -= 0.5*_dx;
    y -= 0.5*_dx;

    int i, j, k;
    double gx, gy, gz;
    Grid3d::positionToGridIndex(x, y, z, _dx, &i, &j, &k);
    Grid3d::GridIndexToPosition(i, j, k, _dx, &gx, &gy, &gz);

    double inv_dx = 1 / _dx;
    double ix = (x - gx)*inv_dx;
    double iy = (y - gy)*inv_dx;
    double iz = (z - gz)*inv_dx;

    int refi = i - 1;
    int refj = j - 1;
    int refk = k - 1;

    double points[4][4][4];
    for (int pk = 0; pk < 4; pk++) {
        for (int pj = 0; pj < 4; pj++) {
            for (int pi = 0; pi < 4; pi++) {
                points[pk][pj][pi] = W(pi + refi, pj + refj, pk + refk);
            }
        }
    }

    return Interpolation::tricubicInterpolate(points, ix, iy, iz);
}

double MACVelocityField::_interpolateLinearU(double x, double y, double z) {
    if (!Grid3d::isPositionInGrid(x, y, z, _dx, _isize, _jsize, _ksize)) {
        return 0.0;
    }

    y -= 0.5*_dx;
    z -= 0.5*_dx;

    int i, j, k;
    double gx, gy, gz;
    Grid3d::positionToGridIndex(x, y, z, _dx, &i, &j, &k);
    Grid3d::GridIndexToPosition(i, j, k, _dx, &gx, &gy, &gz);

    double inv_dx = 1 / _dx;
    double ix = (x - gx)*inv_dx;
    double iy = (y - gy)*inv_dx;
    double iz = (z - gz)*inv_dx;

    double oor = (double)_outOfRangeVector.x;
    double points[8] = {oor, oor, oor, oor, oor, oor, oor, oor};
    if (_u.isIndexInRange(i,   j,   k))   { points[0] = _u(i,   j,   k); }
    if (_u.isIndexInRange(i+1, j,   k))   { points[1] = _u(i+1, j,   k); }
    if (_u.isIndexInRange(i,   j+1, k))   { points[2] = _u(i,   j+1, k); }
    if (_u.isIndexInRange(i,   j,   k+1)) { points[3] = _u(i,   j,   k+1); }
    if (_u.isIndexInRange(i+1, j,   k+1)) { points[4] = _u(i+1, j,   k+1); }
    if (_u.isIndexInRange(i,   j+1, k+1)) { points[5] = _u(i,   j+1, k+1); }
    if (_u.isIndexInRange(i+1, j+1, k))   { points[6] = _u(i+1, j+1, k); }
    if (_u.isIndexInRange(i+1, j+1, k+1)) { points[7] = _u(i+1, j+1, k+1); }

    return Interpolation::trilinearInterpolate(points, ix, iy, iz);
}

double MACVelocityField::_interpolateLinearV(double x, double y, double z) {
    if (!Grid3d::isPositionInGrid(x, y, z, _dx, _isize, _jsize, _ksize)) {
        return 0.0;
    }

    x -= 0.5*_dx;
    z -= 0.5*_dx;

    int i, j, k;
    double gx, gy, gz;
    Grid3d::positionToGridIndex(x, y, z, _dx, &i, &j, &k);
    Grid3d::GridIndexToPosition(i, j, k, _dx, &gx, &gy, &gz);

    double inv_dx = 1 / _dx;
    double ix = (x - gx)*inv_dx;
    double iy = (y - gy)*inv_dx;
    double iz = (z - gz)*inv_dx;

    double oor = (double)_outOfRangeVector.y;
    double points[8] = {oor, oor, oor, oor, oor, oor, oor, oor};
    if (_v.isIndexInRange(i,   j,   k))   { points[0] = _v(i,   j,   k); }
    if (_v.isIndexInRange(i+1, j,   k))   { points[1] = _v(i+1, j,   k); }
    if (_v.isIndexInRange(i,   j+1, k))   { points[2] = _v(i,   j+1, k); }
    if (_v.isIndexInRange(i,   j,   k+1)) { points[3] = _v(i,   j,   k+1); }
    if (_v.isIndexInRange(i+1, j,   k+1)) { points[4] = _v(i+1, j,   k+1); }
    if (_v.isIndexInRange(i,   j+1, k+1)) { points[5] = _v(i,   j+1, k+1); }
    if (_v.isIndexInRange(i+1, j+1, k))   { points[6] = _v(i+1, j+1, k); }
    if (_v.isIndexInRange(i+1, j+1, k+1)) { points[7] = _v(i+1, j+1, k+1); }

    return Interpolation::trilinearInterpolate(points, ix, iy, iz);
}

double MACVelocityField::_interpolateLinearW(double x, double y, double z) {
    if (!Grid3d::isPositionInGrid(x, y, z, _dx, _isize, _jsize, _ksize)) {
        return 0.0;
    }

    x -= 0.5*_dx;
    y -= 0.5*_dx;

    int i, j, k;
    double gx, gy, gz;
    Grid3d::positionToGridIndex(x, y, z, _dx, &i, &j, &k);
    Grid3d::GridIndexToPosition(i, j, k, _dx, &gx, &gy, &gz);

    double inv_dx = 1 / _dx;
    double ix = (x - gx)*inv_dx;
    double iy = (y - gy)*inv_dx;
    double iz = (z - gz)*inv_dx;

    double oor = (double)_outOfRangeVector.z;
    double points[8] = {oor, oor, oor, oor, oor, oor, oor, oor};
    if (_w.isIndexInRange(i,   j,   k))   { points[0] = _w(i,   j,   k); }
    if (_w.isIndexInRange(i+1, j,   k))   { points[1] = _w(i+1, j,   k); }
    if (_w.isIndexInRange(i,   j+1, k))   { points[2] = _w(i,   j+1, k); }
    if (_w.isIndexInRange(i,   j,   k+1)) { points[3] = _w(i,   j,   k+1); }
    if (_w.isIndexInRange(i+1, j,   k+1)) { points[4] = _w(i+1, j,   k+1); }
    if (_w.isIndexInRange(i,   j+1, k+1)) { points[5] = _w(i,   j+1, k+1); }
    if (_w.isIndexInRange(i+1, j+1, k))   { points[6] = _w(i+1, j+1, k); }
    if (_w.isIndexInRange(i+1, j+1, k+1)) { points[7] = _w(i+1, j+1, k+1); }

    return Interpolation::trilinearInterpolate(points, ix, iy, iz);
}

vmath::vec3 MACVelocityField::evaluateVelocityAtPosition(vmath::vec3 pos) {
    return evaluateVelocityAtPosition(pos.x, pos.y, pos.z);
}

vmath::vec3 MACVelocityField::evaluateVelocityAtPosition(double x, double y, double z) {
    if (!Grid3d::isPositionInGrid(x, y, z, _dx, _isize, _jsize, _ksize)) {
        return vmath::vec3();
    }

    double xvel = _interpolateU(x, y, z);
    double yvel = _interpolateV(x, y, z);
    double zvel = _interpolateW(x, y, z);

    return vmath::vec3(xvel, yvel, zvel);
}

vmath::vec3 MACVelocityField::evaluateVelocityAtPositionLinear(vmath::vec3 pos) {
    return evaluateVelocityAtPositionLinear(pos.x, pos.y, pos.z);
}

vmath::vec3 MACVelocityField::evaluateVelocityAtPositionLinear(double x, double y, double z) {
    if (!Grid3d::isPositionInGrid(x, y, z, _dx, _isize, _jsize, _ksize)) {
        return vmath::vec3();
    }

    double xvel = _interpolateLinearU(x, y, z);
    double yvel = _interpolateLinearV(x, y, z);
    double zvel = _interpolateLinearW(x, y, z);

    return vmath::vec3(xvel, yvel, zvel);
}

float MACVelocityField::evaluateVelocityAtPositionLinearU(double x, double y, double z) {
    if (!Grid3d::isPositionInGrid(x, y, z, _dx, _isize, _jsize, _ksize)) {
        return 0.0f;
    }

    return _interpolateLinearU(x, y, z);
}

float MACVelocityField::evaluateVelocityAtPositionLinearV(double x, double y, double z) {
    if (!Grid3d::isPositionInGrid(x, y, z, _dx, _isize, _jsize, _ksize)) {
        return 0.0f;
    }

    return _interpolateLinearV(x, y, z);
}

float MACVelocityField::evaluateVelocityAtPositionLinearW(double x, double y, double z) {
    if (!Grid3d::isPositionInGrid(x, y, z, _dx, _isize, _jsize, _ksize)) {
        return 0.0f;
    }

    return _interpolateLinearW(x, y, z);
}

void MACVelocityField::extrapolateVelocityField(ValidVelocityComponentGrid &validGrid, 
                                                int numLayers) {
    
    GridUtils::extrapolateGrid(&_u, &(validGrid.validU), numLayers);
    GridUtils::extrapolateGrid(&_v, &(validGrid.validV), numLayers);
    GridUtils::extrapolateGrid(&_w, &(validGrid.validW), numLayers);
}

// Method adapted from Fluid Engine Development by Doyub Kim
void MACVelocityField::generateCurlAtCellCenter(Array3d<vmath::vec3> &grid) {
    FLUIDSIM_ASSERT(grid.width == _isize && grid.height == _jsize &&  grid.depth == _ksize);
    grid.fill(vmath::vec3());

    float invdx = 1.0 / _dx;
    for(int k = 0; k < _ksize; k++) {
        for(int j = 0; j < _jsize; j++) {
            for(int i = 0; i < _isize; i++) {
                vmath::vec3 left  = evaluateVelocityAtCellCenter((i > 0) ? i - 1 : i,          j,                            k);
                vmath::vec3 right = evaluateVelocityAtCellCenter((i + 1 < _isize) ? i + 1 : i, j,                            k);
                vmath::vec3 down  = evaluateVelocityAtCellCenter(i,                            (j > 0) ? j - 1 : j,          k);
                vmath::vec3 up    = evaluateVelocityAtCellCenter(i,                            (j + 1 < _jsize) ? j + 1 : j, k);
                vmath::vec3 back  = evaluateVelocityAtCellCenter(i,                            j,                            (k > 0) ? k - 1 : k);
                vmath::vec3 front = evaluateVelocityAtCellCenter(i,                            j,                            (k + 1 < _ksize) ? k + 1 : k);

                float fx_ym = down.x;
                float fx_yp = up.x;
                float fx_zm = back.x;
                float fx_zp = front.x;

                float fy_xm = left.y;
                float fy_xp = right.y;
                float fy_zm = back.y;
                float fy_zp = front.y;

                float fz_xm = left.z;
                float fz_xp = right.z;
                float fz_ym = down.z;
                float fz_yp = up.z;

                vmath::vec3 curl(
                    0.5 * invdx * ((fz_yp - fz_ym) - (fy_zp - fy_zm)),
                    0.5 * invdx * ((fx_zp - fx_zm) - (fz_xp - fz_xm)),
                    0.5 * invdx * ((fy_xp - fy_xm) - (fx_yp - fx_ym))
                    );

                grid.set(i, j, k, curl);
            }
        }
    }
}

void MACVelocityField::getCoarseGridDimensions(int *i, int *j, int *k) {
    *i = _isize / 2;
    *j = _jsize / 2;
    *k = _ksize / 2;
}

void MACVelocityField::getFineGridDimensions(int *i, int *j, int *k) {
    *i = _isize * 2;
    *j = _jsize * 2;
    *k = _ksize * 2;
}

bool MACVelocityField::isDimensionsValidForCoarseGridGeneration() {
    return _isize % 2 == 0 || _jsize % 2 == 0 || _ksize % 2 == 0;
}

MACVelocityField MACVelocityField::generateCoarseGrid() {
    FLUIDSIM_ASSERT(_u.isDimensionsValidForCoarseFaceGridGenerationU());
    FLUIDSIM_ASSERT(_v.isDimensionsValidForCoarseFaceGridGenerationV());
    FLUIDSIM_ASSERT(_w.isDimensionsValidForCoarseFaceGridGenerationW());

    double dxcoarse = _dx * 2.0;
    int icoarse = 0; int jcoarse = 0; int kcoarse = 0;
    getCoarseGridDimensions(&icoarse, &jcoarse, &kcoarse);
    MACVelocityField coarseMAC(icoarse, jcoarse, kcoarse, dxcoarse);

    Array3d<float> *coarseU = coarseMAC.getArray3dU();
    Array3d<float> *coarseV = coarseMAC.getArray3dV();
    Array3d<float> *coarseW = coarseMAC.getArray3dW();

    _u.generateCoarseFaceGridU(*coarseU);
    _v.generateCoarseFaceGridV(*coarseV);
    _w.generateCoarseFaceGridW(*coarseW);

    return coarseMAC;
}

MACVelocityField MACVelocityField::generateFineGrid() {
    double dxfine = _dx / 2.0;
    int ifine = 0; int jfine = 0; int kfine = 0;
    getFineGridDimensions(&ifine, &jfine, &kfine);
    MACVelocityField fineMAC(ifine, jfine, kfine, dxfine);

    Array3d<float> *fineU = fineMAC.getArray3dU();
    Array3d<float> *fineV = fineMAC.getArray3dV();
    Array3d<float> *fineW = fineMAC.getArray3dW();

    for (int k = 0; k < fineU->depth; k++) {
        for (int j = 0; j < fineU->height; j++) {
            for (int i = 0; i < fineU->width; i++) {
                vmath::vec3 gp = Grid3d::FaceIndexToPositionU(i, j, k, dxfine);
                float u = _interpolateLinearU(gp.x, gp.y, gp.z);
                fineU->set(i, j, k, u);
            }
        }
    }

    for (int k = 0; k < fineV->depth; k++) {
        for (int j = 0; j < fineV->height; j++) {
            for (int i = 0; i < fineV->width; i++) {
                vmath::vec3 gp = Grid3d::FaceIndexToPositionV(i, j, k, dxfine);
                float v = _interpolateLinearV(gp.x, gp.y, gp.z);
                fineV->set(i, j, k, v);
            }
        }
    }

    for (int k = 0; k < fineW->depth; k++) {
        for (int j = 0; j < fineW->height; j++) {
            for (int i = 0; i < fineW->width; i++) {
                vmath::vec3 gp = Grid3d::FaceIndexToPositionW(i, j, k, dxfine);
                float w = _interpolateLinearW(gp.x, gp.y, gp.z);
                fineW->set(i, j, k, w);
            }
        }
    }

    return fineMAC;
}