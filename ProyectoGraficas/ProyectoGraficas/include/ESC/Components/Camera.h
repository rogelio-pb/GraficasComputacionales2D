#pragma once
#include "ESC/Types.h"

//ECS :: Components/Camera.h
//
//Componente de datos putos convierte a una entidad
//en una camara, su pocision vive en el transform de la misma
//entidad
//
//en el camera system busca la camaa activa, interpola su 
//transform hacia el objetivo (si lo hay) y aplica la vista
//a la ventana 
//==============================================================

namespace ECS {
	struct Camera {

		//zoom: > 1 acerca (ves menos mundo), <1 aleja(ves mas)
		float zoom{ 1.f };

		//solo UNA camara deberia estar a la vez
		bool active{ true };

		//Entidad a seguir. Null_ENTITY = camara libre(no sigue a naidie)
		EntityID followTarget{ NULL_ENTITY };

		//velocidad de seguimiento. mayor=se pega mas rapido al objetivo
		// Es framerate-independent (ver camerasystem) no es px/seg
		float followSpeed{ 5.f };

	};
}//namespace  ECS