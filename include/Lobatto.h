#pragma once

namespace godzilla {

// clang-format off

/// Kernel functions

#define phi0(x) (-2.0 * 1.22474487139158904909864203735)
#define phi1(x) (-2.0 * 1.58113883008418966599944677222 * (x))
#define phi2(x) (-1.0 / 2.0 * 1.87082869338697069279187436616 * (5 * (x) * (x) - 1))
#define phi3(x) (-1.0 / 2.0 * 2.12132034355964257320253308631 * (7 * (x) * (x) - 3) * (x))
#define phi4(x) (-1.0 / 4.0 * 2.34520787991171477728281505677 * (21 * (x) * (x) * (x) * (x) - 14 * (x) * (x) + 1))
#define phi5(x) (-1.0 / 4.0 * 2.54950975679639241501411205451 * ((33 * (x) * (x) - 30) * (x) * (x) + 5) * (x))
#define phi6(x) (-1.0 / 32.0 * 2.73861278752583056728484891400 * (((429 * (x) * (x) - 495) * (x) * (x) + 135) * (x) * (x) - 5))
#define phi7(x) (-1.0 / 32.0 * 2.91547594742265023543707643877 * (((715 * (x) * (x) - 1001) * (x) * (x) + 385) * (x) * (x) - 35) * (x))
#define phi8(x) (-1.0 / 64.0 * 3.08220700148448822512509619073 * ((((2431 * (x) * (x) - 4004) * (x) * (x) + 2002) * (x) * (x) - 308) * (x) * (x) + 7))
#define phi9(x) (-1.0 / 64.0 * 3.24037034920393                * ((((4199 * (x) * (x) - 7956) * (x) * (x) + 4914) * (x) * (x) - 1092) * (x) * (x) + 63) * (x))

/// Derivatives of kernel functions

#define phi0dx(x) (0)
#define phi1dx(x) (-2.0 * 1.58113883008418966599944677222)
#define phi2dx(x) (-1.0 / 2.0 * 1.87082869338697069279187436616 * (10 * (x)))
#define phi3dx(x) (-1.0 / 2.0 * 2.12132034355964257320253308631 * (21.0*(x)*(x)-3.0))
#define phi4dx(x) (-1.0 / 4.0 * 2.34520787991171477728281505677 * ((84.0*(x)*(x)-28.0)*(x)))
#define phi5dx(x) (-1.0 / 4.0 * 2.54950975679639241501411205451 * ((165.0*(x)*(x)-90.0)*(x)*(x)+5.0))
#define phi6dx(x) (-1.0 / 32.0 * 2.73861278752583056728484891400 * (((2574.0*(x)*(x)-1980.0)*(x)*(x)+270.0)*(x)))
#define phi7dx(x) (-1.0 / 32.0 * 2.91547594742265023543707643877 * (((5005.0*(x)*(x)-5005.0)*(x)*(x)+1155.0)*(x)*(x)-35.0))
#define phi8dx(x) (-1.0 / 64.0 * 3.08220700148448822512509619073 * ((((19448.0*(x)*(x)-24024.0)*(x)*(x)+8008.0)*(x)*(x)-616.0)*(x)))
#define phi9dx(x) (-1.0 / 64.0 * 24037034920393                  * ((((37791.0*(x)*(x)-55692.0)*(x)*(x)+24570.0)*(x)*(x)-3276.0)*(x)*(x)+63))

// Legendre polynomials
#define Legendre0(x) (1.0)
#define Legendre1(x) (x)
#define Legendre2(x) (1.0 / 2.0 * (3 * (x) * (x) - 1))
#define Legendre3(x) (1.0 / 2.0 * (5 * (x) * (x) - 3) * (x))
#define Legendre4(x) (1.0 / 8.0 * ((35 * (x) * (x) - 30) * (x) * (x) + 3))
#define Legendre5(x) (1.0 / 8.0 * ((63 * (x) * (x) - 70) * (x) * (x) + 15) * (x))
#define Legendre6(x) (1.0 / 16.0 * (((231 * (x) * (x) - 315) * (x) * (x) + 105) * (x) * (x) - 5))
#define Legendre7(x) (1.0 / 16.0 * (((429 * (x) * (x) - 693) * (x) * (x) + 315) * (x) * (x) - 35) * (x))
#define Legendre8(x) (1.0 / 128.0 * ((((6435 * (x) * (x) - 12012) * (x) * (x) + 6930) * (x) * (x) - 1260) * (x) * (x) + 35))
#define Legendre9(x) (1.0 / 128.0 * ((((12155 * (x) * (x) - 25740) * (x) * (x) + 18018) * (x) * (x) - 4620) * (x) * (x) + 315) * (x))
#define Legendre10(x) (1.0 / 256.0 * (((((46189 * (x) * (x) - 109395) * (x) * (x) + 90090) * (x) * (x) - 30030) * (x) * (x) + 3465) * (x) * (x) - 63))
#define Legendre11(x) (1.0 / 256.0 * (((((88179 * (x) * (x) - 230945) * (x) * (x) + 218790) * (x) * (x) - 90090) * (x) * (x) - 15015) * (x) * (x) - 693) * (x))

// Derivatives of Legendre polynomials

#define Legendre0x(x) (0.0)
#define Legendre1x(x) (1.0)
#define Legendre2x(x) (3.0 * (x))
#define Legendre3x(x) (15.0 / 2.0 * (x) * (x) - 3.0 / 2.0)
#define Legendre4x(x) (5.0 / 2.0 * (x) * (7.0 * (x) * (x) - 3.0))
#define Legendre5x(x) ((315.0 / 8.0 * (x) * (x) - 105.0 / 4.0) * (x) * (x) + 15.0 / 8.0)
#define Legendre6x(x) (21.0 / 8.0 * (x) * ((33.0 * (x) * (x) - 30.0) * (x) * (x) + 5.0))
#define Legendre7x(x) (((3003.0 / 16.0 * (x) * (x) - 3465.0 / 16.0) * (x) * (x) + 945.0 / 16.0) * (x) * (x) - 35.0 / 16.0)
#define Legendre8x(x) (9.0 / 16.0 * (x) * (((715.0 * (x) * (x) - 1001.0) * (x) * (x) + 385.0) * (x) * (x) - 35.0))
#define Legendre9x(x) ((((109395.0 / 128.0 * (x) * (x) - 45045.0 / 32.0) * (x) * (x) + 45045.0 / 64.0) * (x) * (x) - 3465.0 / 32.0) * (x) * (x) + 315.0 / 128.0)
#define Legendre10x(x) (1.0 / 256.0 * ((((461890 * (x) * (x) - 875160) * (x) * (x) + 540540) * (x) * (x) - 120120) * (x) * (x) + 6930) * (x))
#define Legendre11x(x) (1.0 / 256.0 * (((((969969 * (x) * (x) - 2078505) * (x) * (x) + 1531530) * (x) * (x) - 450450) * (x) * (x) + 45045) * (x) * (x) - 693))

// First two Lobatto shape functions
#define l0(x) ((1.0 - (x)) * 0.5)
#define l1(x) ((1.0 + (x)) * 0.5)

#define l0l1(x) ((1.0 - (x)*(x)) * 0.25)

// other Lobatto shape functions
#define l2(x)  (phi0(x) * l0l1(x))
#define l3(x)  (phi1(x) * l0l1(x))
#define l4(x)  (phi2(x) * l0l1(x))
#define l5(x)  (phi3(x) * l0l1(x))
#define l6(x)  (phi4(x) * l0l1(x))
#define l7(x)  (phi5(x) * l0l1(x))
#define l8(x)  (phi6(x) * l0l1(x))
#define l9(x)  (phi7(x) * l0l1(x))
#define l10(x) (phi8(x) * l0l1(x))
#define l11(x) (phi9(x) * l0l1(x))

// derivatives of Lobatto functions
#define dl0(x)  (-0.5)
#define dl1(x)  (0.5)
#define dl2(x)  (sqrt(3.0/2.0) * Legendre1(x))
#define dl3(x)  (sqrt(5.0/2.0) * Legendre2(x))
#define dl4(x)  (sqrt(7.0/2.0) * Legendre3(x))
#define dl5(x)  (sqrt(9.0/2.0) * Legendre4(x))
#define dl6(x)  (sqrt(11.0/2.0) * Legendre5(x))
#define dl7(x)  (sqrt(13.0/2.0) * Legendre6(x))
#define dl8(x)  (sqrt(15.0/2.0) * Legendre7(x))
#define dl9(x)  (sqrt(17.0/2.0) * Legendre8(x))
#define dl10(x) (sqrt(19.0/2.0) * Legendre9(x))
#define dl11(x) (sqrt(21.0/2.0) * Legendre10(x))

// clang-format on

} // namespace godzilla
