
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_GUI_INPUTSYSTEM_DEBOUNCER__
#define __SHIVA_GUI_INPUTSYSTEM_DEBOUNCER__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include <sstream>
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

namespace ShivaGUI
{

	/// This keeps track of a single input source
	/// How's this going to work?
	/// * input -> inputPotential = true and store timestamp, activate output if leading edge
	/// * time passes, activate output if delayed leading edge
	/// * no input for certain time -> inputPotential = false
	///
	/// Need:
	/// * Repeat bool - if it will repeat output if continuously pressed
	/// * Repeat delay - time of continuous activation before repeat starts
	/// * Repeat period - time between outputs of repeat
	///
	/// * also need option to suppress actual input after a block has completed for a period of time
	class Debouncer
	{
	public:

		enum Phase
		{
			INACTIVE,	// There is no input
			ACTIVE,		// Inputs are going on
			TRAILING,	// Actual inputs have stopped but we are still active
			SUPPRESSING	// We are inactive and are preventing inputs from reactivating us
		};

		enum OutputType
		{
			OUT_NONE = 0,	// There is no active output
			OUT_LEADING,	// The output is the leading edge of the activation
			OUT_DELAYED,	// The output is a delayed response from the leading edge
			OUT_TRAILING,	// The output is trailing
			OUT_REPEATING	// The output is a repeating one
		};

		Debouncer();

		void SetTrailTime( float timeS ) {_trailMaxTime=timeS;}//{_trailTimeTicks = 1000 * timeS;}
		void SetSuppressionTime( float timeS ) {_suppressionMaxTime=timeS;}//{_suppressionTicks = 1000 * timeS;}
		void SetDelayTime( float timeS ) {_delayedOutputMaxTime=timeS;}//{_delayedOutputTicks = 1000 * timeS;}

		//void SetOutputState(int state) {SetOutputState(state==1,state==2,state==3);}
		//void SetOutputState(bool leading, bool delayed, bool trailing) {_outputLeading=leading; _outputDelayed=delayed; _outputTrailing=trailing;}

		void SetRepeat(bool enable, float rateTimeS, float delayTimeS, bool repeatDebounced ) {_repeatEnable=enable; _repeatRateTime = rateTimeS; _repeatDelayTime = delayTimeS; _repeatDebounced=repeatDebounced;}

		/// Processes the input
		/// Returns true if an event should be issued
		OutputType Update(float deltaTs);

		void SetInput(bool value);



	protected:

		/// Settings
			/// Number of system clock ticks between the last input and the trailing edge of activation
			//unsigned int _trailTimeTicks;
			float _trailMaxTime;

			/// Number of ticks after an activation period to prevent reactivation
			//unsigned int _suppressionTicks;
			float _suppressionMaxTime;

			/// Time from start of input to issuing an output event
			//unsigned int _delayedOutputTicks;
			float _delayedOutputMaxTime;

			bool _repeatEnable;

			/// Will allow the debounced output to trigger a repeat (i.e. a messy long input will be treated as a single press)
			/// default is false
			bool _repeatDebounced;

			/// Time between issuing repeat events, after the initial repeat delay
			float _repeatRateTime;

			/// Time of continued input before first repeat event
			float _repeatDelayTime;

		/// Variables

			Phase _currentPhase;


			/// If we are timing the block
			//bool _timingBlock;
			/// Timer for the block
			float _blockTimer;

			/// If we are timing a trail
			//bool _timingTrail;
			/// Timer for the trail
			float _trailTimer;

			bool _timingDelay;

			float _delayTimer;

			float _suppressionTimer;


			/// Tracks the actual input
			bool _inputPotential;

			/// Flags that are set when giving output
			bool _leadingEdgeOutput, _delayedLeadOutput, _trailingEdgeOutput;



			bool _timingRepeat;

			bool _firstRepeat;

			float _repeatTimer;

			void StartRepeatTimer();

/*
			/// If we are timing a delayed output
			bool _delaying;


			/// Number of system clock ticks since the current input 'block' started and stopped
			//unsigned int _blockStartTicks, _blockTrailingTicks, _blockStopTicks;
			float _blockTimer, _blockTrailingTimer, _blockStopTimer;

			/// Used when repeat doesn't follow the debounced block
			//unsigned int _repeatStartTicks;

			/// Counter for issuing repeat outputs
			unsigned int _repeatOutputCount;
*/
			void DebugOutput(std::string);

	};

}

//////////////////////////////////////////////////////////////////////////
#endif
