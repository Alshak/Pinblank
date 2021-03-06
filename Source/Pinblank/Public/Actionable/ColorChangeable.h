// Copyright Alain SHAKOUR 2016 contact@alainshakour.com This software is a computer program whose purpose is entertainment.This software is governed by the CeCILL-C license under French law and abiding by the rules of distribution of free software.  You can  use, modify and/ or redistribute the software under the terms of the CeCILL-C license as circulated by CEA, CNRS and INRIA at the following URL "http://www.cecill.info". As a counterpart to the access to the source code and  rights to copy, modify and redistribute granted by the license, users are provided only with a limited warranty  and the software's author,  the holder of the economic rights,  and the successive licensors  have only  limited liability. In this respect, the user's attention is drawn to the risks associated with loading,  using,  modifying and/or developing or reproducing the software by the user in light of its specific status of free software, that may mean  that it is complicated to manipulate,  and  that  also therefore means  that it is reserved for developers  and  experienced professionals having in-depth computer knowledge. Users are therefore encouraged to load and test the software's suitability as regards their requirements in conditions enabling the security of their systems and/or data to be ensured and,  more generally, to use and operate it in the same conditions as regards security. The fact that you are presently reading this means that you have had knowledge of the CeCILL-C license and that you accept its terms.

#pragma once

#include "Object.h"
#include "ColorChangeable.generated.h"

UINTERFACE()
class PINBLANK_API UColorChangeable : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class PINBLANK_API IColorChangeable
{
	GENERATED_IINTERFACE_BODY()

	UMaterialInstanceDynamic* dynamicMaterialInstance;

	virtual UStaticMeshComponent* GetColoredMesh() PURE_VIRTUAL(IColorChangeable::GetColoredMesh, return nullptr;);
	virtual const FName GetMaterialParameterColorName() const PURE_VIRTUAL(IColorChangeable::GetMaterialParameterColorName, return TEXT("ParamColor"););

	void InitializeDynamicMaterialInstance(UObject* obj);

	public:
		void ChangeColor(FLinearColor color);
};
