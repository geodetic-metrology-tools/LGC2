format long

RAD_TO_GONZ = 2 * pi / 400;

%PARAMETERS OF THE TRANSFORMATION
omega = 77 * RAD_TO_GONZ
phi = -56 * RAD_TO_GONZ
kappa = 195 * RAD_TO_GONZ

translation = [-606; -1291; -1]
scale = 1


FPT2 = [cos(kappa)*cos(phi) , sin(kappa)*cos(phi) , - sin(phi) ,  translation(1)/scale;
cos(kappa)*sin(phi)*sin(omega)-sin(kappa)*cos(omega), sin(kappa)*sin(phi)*sin(omega)+cos(kappa)*cos(omega) , cos(phi)*sin(omega) ,translation(2)/scale;
cos(kappa)*sin(phi)*cos(omega)+sin(kappa)*sin(omega),  sin(kappa)*sin(phi)*cos(omega)-cos(kappa)*sin(omega)  , cos(phi)*cos(omega),  translation(3)/scale;
0 , 0 , 0 , 1/scale]
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
omega = 32 * RAD_TO_GONZ
phi = 5 * RAD_TO_GONZ
kappa = -148 * RAD_TO_GONZ

translation = [150; 9758; -160]
scale = 2

FREF2 = [cos(kappa)*cos(phi) , sin(kappa)*cos(phi) , - sin(phi) ,  translation(1)/scale;
cos(kappa)*sin(phi)*sin(omega)-sin(kappa)*cos(omega), sin(kappa)*sin(phi)*sin(omega)+cos(kappa)*cos(omega) , cos(phi)*sin(omega) ,translation(2)/scale;
cos(kappa)*sin(phi)*cos(omega)+sin(kappa)*sin(omega),  sin(kappa)*sin(phi)*cos(omega)-cos(kappa)*sin(omega)  , cos(phi)*cos(omega),  translation(3)/scale;
0 , 0 , 0 , 1/scale]

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
omega = 3 * RAD_TO_GONZ
phi = -201 * RAD_TO_GONZ
kappa = 19 * RAD_TO_GONZ

translation = [06; -191; 1561]
scale = 1.005

FREF = [cos(kappa)*cos(phi) , sin(kappa)*cos(phi) , - sin(phi) ,  translation(1)/scale;
cos(kappa)*sin(phi)*sin(omega)-sin(kappa)*cos(omega), sin(kappa)*sin(phi)*sin(omega)+cos(kappa)*cos(omega) , cos(phi)*sin(omega) ,translation(2)/scale;
cos(kappa)*sin(phi)*cos(omega)+sin(kappa)*sin(omega),  sin(kappa)*sin(phi)*cos(omega)-cos(kappa)*sin(omega)  , cos(phi)*cos(omega),  translation(3)/scale;
0 , 0 , 0 , 1/scale];

FREF = inv(FREF)


POINT = [-10.597132 ; 0.005198 ; 4.55658 ; 1]

% FPT2_TO_FREF Transformation
FPT2_TO_FREF = FREF*FREF2*FPT2
FPT2_TO_FREF_TRANSFORMED = (FPT2_TO_FREF*POINT)
FPT2_TO_FREF_TRANSFORMED/=FPT2_TO_FREF_TRANSFORMED(4)

% FREF_TO_FPT2 Transformation
FREF = inv(FREF)
FREF2 = inv(FREF2)
FPT2 = inv(FPT2)
FREF_TO_FPT2 = FPT2*FREF2*FREF
FREF_TO_FPT2_TRANSFORMED= (FREF_TO_FPT2*FPT2_TO_FREF_TRANSFORMED)
FREF_TO_FPT2_TRANSFORMED/=FREF_TO_FPT2_TRANSFORMED(4)
