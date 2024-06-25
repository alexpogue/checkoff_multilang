<template>
  <div class="todo_list">
    <ul>
      <li
        v-for="item in todoItems"
        v-bind:key="item.id"
      >
      {{ item.title }}
      </li>
    </ul>
  </div>
</template>

<script>
export default {
  name: 'HelloWorld',
  props: {
    todoItems: Array
  }
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
h3 {
  margin: 40px 0 0;
}
</style>
