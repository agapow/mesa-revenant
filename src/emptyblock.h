#ifndef __EMPTYBLOCK_H
#define __EMPTYBLOCK_H

#include "nexus.h" // PMA

class EmptyBlock : public NexusBlock
{
   // Adding a new data member? Don't forget to:
   // 1. Describe it in the class header comment at the top of "emptyblock.cpp"
   // 2. Initialize it (unless it is self-initializing) in the constructor
   //    and reinitialize it in the Reset function
   // 3. Describe the initial state in the constructor documentation
   // 4. Delete memory allocated to it in both the destructor and Reset function
   // 5. Report it in some way in the Report function

protected:
	int CharLabelToNumber ( nxsstring s ); // PMA
   void HandleEndblock( NexusToken& token );
	virtual void Read( NexusToken& token );
	virtual void Reset();

public:
	EmptyBlock();
   ~EmptyBlock();

	virtual void Report( ostream& out );
};

#endif

