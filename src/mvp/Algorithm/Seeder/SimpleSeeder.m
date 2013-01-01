function self = SimpleSeeder(_georef, _size)
  self = MvpClass();

  self._georef = _georef;
  self._size = _size;

  self._curr_post = pixel2post(self._georef, (_size - 1) / 2);
  self._curr_seed = AlgorithmVar([-1000+_georef.datum().semi_major_axis();
                                  tanplane(self._curr_post); [30;30]; [0;0];
                                  0; 0; 60]);
  self._done = 0;
  self._result = {};

  self.curr_post = @(self) self._curr_post;
  self.curr_seed = @(self) self._curr_seed;
  self.done = @(self) self._done; 
  self.result = @(self) self._result;
  self.update = @update;
endfunction

function update(self, new_result)
  self._result = {Seed(self._curr_post, new_result)};
  self._done = 1;
endfunction

% vim:set syntax=octave:
