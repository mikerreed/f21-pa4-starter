#ifndef GPath_DEFINED
#define GPath_DEFINED

#include <vector>
#include "GMatrix.h"
#include "GPoint.h"
#include "GRect.h"

class GPath {
public:
    GPath();
    ~GPath();

    GPath& operator=(const GPath&);

    /**
     *  Erase any previously added points/verbs, restoring the path to its initial empty state.
     */
    GPath& reset();

    /**
     *  Start a new contour at the specified coordinate.
     */
    GPath& moveTo(GPoint p) {
        fPts.push_back(p);
        fVbs.push_back(kMove);
        return *this;
    }
    GPath& moveTo(float x, float y) { return this->moveTo({x, y}); }

    /**
     *  Connect the previous point (either from a moveTo or lineTo) with a line segment to
     *  the specified coordinate.
     */
    GPath& lineTo(GPoint p) {
        assert(fVbs.size() > 0);
        fPts.push_back(p);
        fVbs.push_back(kLine);
        return *this;
    }
    GPath& lineTo(float x, float y) { return this->lineTo({x, y}); }

    enum Direction {
        kCW_Direction,
        kCCW_Direction,
    };
    
    /**
     *  Append a new contour, made up of the 4 points of the specified rect, in the specified
     *  direction. The contour will begin at the top-left corner of the rect.
     */
    GPath& addRect(const GRect&, Direction = kCW_Direction);

    /**
     *  Append a new contour with the specified polygon. Calling this is equivalent to calling
     *  moveTo(pts[0]), lineTo(pts[1..count-1]).
     */
    GPath& addPolygon(const GPoint pts[], int count);

    int countPoints() const { return (int)fPts.size(); }

    /**
     *  Return the bounds of all of the control-points in the path.
     *
     *  If there are no points, return {0, 0, 0, 0}
     */
    GRect bounds() const;

    /**
     *  Transform the path in-place by the specified matrix.
     */
    void transform(const GMatrix&);

    void offset(float dx, float dy) {
        this->transform(GMatrix::Translate(dx, dy));
    }

    enum Verb {
        kMove,  // returns pts[0] from Iter
        kLine,  // returns pts[0]..pts[1] from Iter
        kDone   // returns nothing in pts, Iter is done
    };

    // maximum number of points returned by Iter::next() and Edger::next()
    enum {
        kMaxNextPoints = 2
    };

    /**
     *  Walks the path, returning each verb that was entered.
     *  e.g.    moveTo() returns kMove
     *          lineTo() returns kLine
     */
    class Iter {
    public:
        Iter(const GPath&);
        Verb next(GPoint pts[]);

    private:
        const GPoint* fPrevMove;
        const GPoint* fCurrPt;
        const Verb*   fCurrVb;
        const Verb*   fStopVb;
    };

    /**
     *  Walks the path, returning "edges" only. Thus it does not return kMove, but will return
     *  the final closing "edge" for each contour.
     *  e.g.
     *      path.moveTo(A).lineTo(B).lineTo(C).moveTo(D).lineTo(E)
     *  will return
     *      kLine   A..B
     *      kLine   B..C
     *      kLine   C..A
     *      kLine   D..E
     *      kLine   E..D
     *      kDone
     */
    class Edger {
    public:
        Edger(const GPath&);
        Verb next(GPoint pts[]);

    private:
        const GPoint* fPrevMove;
        const GPoint* fCurrPt;
        const Verb*   fCurrVb;
        const Verb*   fStopVb;
        Verb fPrevVerb;
    };

    void dump() const;

private:
    std::vector<GPoint> fPts;
    std::vector<Verb>   fVbs;
};

#endif

