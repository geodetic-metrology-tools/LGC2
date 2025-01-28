Adaption of the SCT testfile for the integration of a sag computation.

Thoughts:
The sag should be computed using the wire sensors.
The position of each wire sensor should be known in advance in the straight configuration.
Via observations of the sensors in bent configuration, the sag can be computed as the parameter that modifies the sensor positions in such a way vertically such that the bent observations are explained best.

In the SCT file there are 8 wire sensors:


*ECWI	WPS1	0.0003589	0.0000000	SCT.GISCD.A1LX.D_WPS	SCT.GISCD.B1LX.D_WPS	WIID	GIWPN.A1LX.Q
SCT.GTAP.A1LX.D_WPS	-0.001155	-0.034158	ID	SCT.GTAP.A1LX.D_WPS
SCT.GTAP.A1LX.Q_WPS	-0.003519	-0.034428	ID	SCT.GTAP.A1LX.Q_WPS
SCT.GISCD.A1LX.D_WPS	-0.004649	-0.034501	ID	SCT.GISCD.A1LX.D_WPS
SCT.GISCD.B1LX.D_WPS	-0.000053	-0.034469	ID	SCT.GISCD.B1LX.D_WPS
*ECWI	WPS1	0.0003592	0.0000000	SCT.GISCD.A1LX.A_WPS	SCT.GISCD.B1LX.A_WPS	WIID	GIWPN.A1LX.T
SCT.GTAP.A1LX.L_WPS	-0.002074	-0.034657	ID	SCT.GTAP.A1LX.L_WPS
SCT.GTAP.A1LX.A_WPS	-0.000604	-0.034249	ID	SCT.GTAP.A1LX.A_WPS
SCT.GISCD.A1LX.A_WPS	-0.002523	-0.034408	ID	SCT.GISCD.A1LX.A_WPS
SCT.GISCD.B1LX.A_WPS	0.000145	-0.034530	ID	SCT.GISCD.B1LX.A_WPS

So the position of these 8 sensors either has to be known in advance in unbent status (e.g. via obsxyz)
Or there has to be a new set of point names representing the same sensors in bent status measuring during bent status.
Q: take care what happens to the wire (LGC implementation?)
The point positions are defined via frames in the sct file -> do these frames represent bent or not bent status?
Do we just introduce duplicate points, like 
SCT.GTAP.A1LX.D_WPS_sagged
and measure this in bent status
the line 
sag_connect(SCT.GTAP.A1LX.D_WPS,SCT.GTAP.A1LX.D_WPS_sagged,sag_oject)
will tell lgc that both points are connected via the sag element
The point SCT.GTAP.A1LX.D_WPS_sagged should be introduced as free point.
Its position then is determined fully by the position of SCT.GTAP.A1LX.D_WPS and the sag object (containing curvature, slack etc)


Plan for a mockup:
-compute the positions of the original points
-based on the positions of the original points compute their offset from the x-axis in the deciding frame
-based on the offset and a given curvature and slack compute their sagged position
-with the sagged position, compute what would be the observed value of the wire sensor
-add the sagged points beside the normal points in the input file -as free points, connect them to the original points via the sag element
-include the simulated wire observation values as observations on these points
-> lgc should reconstruct the sag parameters
-> to see the effect of the sag on arbitrary other points there should be a function that can compute the new position?


Start preparing SCT_file_Brad.lgc by adding some commented out lines. To be discussed with Vivien.

