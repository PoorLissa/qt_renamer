#ifndef __BITSET_H
#define __BITSET_H

// класс битового поля. содержит не более 64 полей (0-63)
class bitSet {
 public:
	bitSet() {
		bit1 = 0;
		bit2 = 0;
	}

	void bitOn(const unsigned int pos) {
		if( pos >= 0 ) {
			if( pos < 32 )
				bit1 |= (1 << pos);
			else if ( pos < 64 )
				bit2 |= (1 << (pos-32));
		}
	}

	void bitOff(const unsigned int pos) {
		if( pos >= 0 ) {
			if( pos < 32 )
				bit1 &= ~(1 << pos);
			else if( pos < 64 )
				bit2 &= ~(1 << (pos-32));
		}
	}

	void SetAll(const bool val) {
		if( val ) {
			bit1 = ~0;
			bit2 = ~0;
		}
		else {
			bit1 = 0;
			bit2 = 0;
		}
	}

	bool getBit(const unsigned int pos) {
		if( pos >= 0 ) {
			if( pos < 32 )
				return bit1 & (1 << pos);
			else if( pos < 64 )
				return bit2 & (1 << (pos-32));
			else
				return false;
		}
	}

 private:
	unsigned long bit1;
	unsigned long bit2;
};

#endif