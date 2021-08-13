#ifndef _SDF_GLSL
#define _SDF_GLSL

/*
 * Largely taken or inspired by https://iquilezles.untergrund.net/www/articles/distfunctions2d/distfunctions2d.htm

MIT License. Inigo Quilez, Edgar Simo-Serra

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
 * Helper Functions.
 */
float cro( vec2 a, vec2 b ) { return a.x*b.y - a.y*b.x; }


/* Circle. */
float sdCircle( vec2 p, float r )
{
   return length(p)-r;
}

/* Box at position b with border b. */
float sdBox( vec2 p, vec2 b )
{
   vec2 d = abs(p)-b;
   return length(max(d,0.0)) + min(max(d.x,d.y),0.0);
}

/* Uneven capsule oriented on Y axis. */
float sdUnevenCapsuleY( vec2 p, float ra, float rb, float h )
{
	p.x = abs(p.x);

   float b = (ra-rb)/h;
   vec2  c = vec2(sqrt(1.0-b*b),b);
   float k = cro(c,p);
   float m = dot(c,p);
   float n = dot(p,p);

        if( k < 0.0   ) return sqrt(n)               - ra;
   else if( k > c.x*h ) return sqrt(n+h*h-2.0*h*p.y) - rb;
                        return m                     - ra;
}

/* Uneven capsule between points pa and pb. */
float sdUnevenCapsule( vec2 p, vec2 pa, vec2 pb, float ra, float rb )
{
    p  -= pa;
    pb -= pa;
    float h = dot(pb,pb);
    vec2  q = vec2( dot(p,vec2(pb.y,-pb.x)), dot(p,pb) )/h;

    q.x = abs(q.x);

    float b = ra-rb;
    vec2  c = vec2(sqrt(h-b*b),b);

    float k = cro(c,q);
    float m = dot(c,q);
    float n = dot(q,q);

         if( k < 0.0 ) return sqrt(h*(n            )) - ra;
    else if( k > c.x ) return sqrt(h*(n+1.0-2.0*q.y)) - rb;
                       return m                       - ra;
}



#endif /* _SDF_GLSL */
