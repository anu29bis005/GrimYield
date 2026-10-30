#pragma once

#include "DrawDebugHelpers.h"

#define DEBUG_SPHERE(Location) if(GetWorld()) DrawDebugSphere(GetWorld(), Location, 25.f, 12, FColor::Magenta, true);
#define DEBUG_SPHERE_PERS(Location) if(GetWorld()) DrawDebugSphere(GetWorld(), Location, 5.f, 12, FColor::Yellow, false, 5.f);
#define DEBUG_SPHERE_SingleFrame(Location) if(GetWorld()) DrawDebugSphere(GetWorld(), Location, 25.f, 12, FColor::Magenta, false, -1.f);

#define DEBUG_LINE(StartLine, EndLine) if(GetWorld()) DrawDebugLine(GetWorld(), StartLine, EndLine, FColor::Yellow, true, -1.f, 0, 1.f);
#define DEBUG_LINE_SingleFrame(StartLine, EndLine) if(GetWorld()) DrawDebugLine(GetWorld(), StartLine, EndLine, FColor::Yellow, false, -1.f, 0, 1.f);

#define DEBUG_POINT(Location) if(GetWorld()) DrawDebugPoint(GetWorld(), Location, 15.f, FColor::Red, true);
#define DEBUG_POINT_SingleFrame(Location) if(GetWorld()) DrawDebugPoint(GetWorld(), Location, 15.f, FColor::Red, false, -1.f);

#define DEBUG_VECTOR(StartLocation, EndLocation) if(GetWorld()) \
{ \
	DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, true, -1.f, 0, 1.f); \
	DrawDebugPoint(GetWorld(), EndLocation, 15.f, FColor::Green, true); \
}
#define DEBUG_VECTOR_SingleFrame(StartLocation, EndLocation) if(GetWorld()) \
{ \
	DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, -1.f, 0, 1.f); \
	DrawDebugPoint(GetWorld(), EndLocation, 15.f, FColor::Green, false, -1.f); \
}